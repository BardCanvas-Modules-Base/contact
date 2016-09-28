<?php
namespace hng2_modules\contact;

use hng2_repository\abstract_repository;

class pms_repository extends abstract_repository
{
    protected $row_class       = "hng2_modules\\contact\\pm_record";
    protected $table_name      = "pms";
    protected $key_column_name = "id_pm";
    
    /**
     * @param $id
     *
     * @return pm_record|null
     */
    public function get($id)
    {
        return parent::get($id);
    }
    
    /**
     * @param array  $where
     * @param int    $limit
     * @param int    $offset
     * @param string $order
     *
     * @return pm_record[]
     */
    public function find($where, $limit, $offset, $order)
    {
        return parent::find($where, $limit, $offset, $order);
    }
    
    /**
     * @param pm_record $record
     *
     * @return int
     */
    public function save($record)
    {
        global $database;
        
        $this->validate_record($record);
        $record->sent_date = date("Y-m-d H:i:s");
        $obj = $record->get_for_database_insertion();
        
        $affected_rows = $database->exec("
            insert ignore into pms set
            `id_pm`              = '{$obj->id_pm       }',
            `id_owner`           = '{$obj->id_owner    }',
            `id_sender`          = '{$obj->id_sender   }',
            `id_recipient`       = '{$obj->id_recipient}',
            `sent_date`          = '{$obj->sent_date   }',
            `opened_date`        = '{$obj->opened_date }',
            `contents`           = '{$obj->contents    }'
        ");
        $this->last_query = $database->get_last_query();
        
        return $affected_rows;
    }
    
    /**
     * @param pm_record $record
     *
     * @throws \Exception
     */
    public function validate_record($record)
    {
        if( ! $record instanceof pm_record )
            throw new \Exception(
                "Invalid object class! Expected: {$this->row_class}, received: " . get_class($record)
            );
    }
    
    public function get_unread_count($id_recipient)
    {
        return $this->get_record_count(array(
            "id_recipient" => $id_recipient,
            "opened_date"  => "0000-00-00 00:00:00"
        ));
    }
    
    public function send($id_recipient, $content)
    {
        global $account;
        
        $record = new pm_record(array(
            "id_owner"     => $account->id_account,
            "id_sender"    => $account->id_account,
            "id_recipient" => $id_recipient,
            "sent_date"    => date("Y-m-d H:i:s"),
            "opened_date"  => date("Y-m-d H:i:s"),
            "contents"     => $content,
        ));
        $record->set_new_id();
        $this->save($record);
        
        $record = new pm_record(array(
            "id_owner"     => $id_recipient,
            "id_sender"    => $account->id_account,
            "id_recipient" => $id_recipient,
            "sent_date"    => date("Y-m-d H:i:s"),
            "opened_date"  => "0000-00-00 00:00:00",
            "contents"     => $content,
        ));
        $record->set_new_id();
        $this->save($record);
        
        $this->ping_conversation($record->id_sender,    $record->id_recipient, $record->sent_date);
        $this->ping_conversation($record->id_recipient, $record->id_sender,    $record->sent_date);
    }
    
    public function ping_conversation($id_owner, $id_other, $last_event_date)
    {
        global $database;
        
        $res = $database->exec("
            update pm_conversations
            set last_event_date = '$last_event_date', archived = '0'
            where id_owner = '$id_owner'
            and   id_other = '$id_other'
        ");
        $this->last_query = $database->get_last_query();
        
        $id = make_unique_id("21");
        if( $res == 0 ) $database->exec("
            insert into pm_conversations set
            id_conversation = '$id',
            id_owner        = '$id_owner',
            id_other        = '$id_other',
            last_event_date = '$last_event_date'
        ");
        $this->last_query = $database->get_last_query();
    }
    
    /**
     * @param     $id_owner
     * @param int $archived_flag 0:active 1: archived
     *
     * @return conversation_record[]
     */
    public function get_conversations($id_owner, $archived_flag = 0)
    {
        global $database;
        
        $res = $database->query("
            select * from pm_conversations where id_owner = '$id_owner'
            and archived = '$archived_flag'
            order by last_event_date desc
        ");
        $this->last_query = $database->get_last_query();
        if( $database->num_rows($res) == 0 ) return array();
        
        $return = array();
        while($row = $database->fetch_object($res))
            $return[] = new conversation_record($row);
        
        return $return;
    }
    
    public function get_conversations_count($id_owner, $archived_flag = 0)
    {
        global $database;
        
        $res = $database->query("
            select count(*) as `count` from pm_conversations
            where id_owner = '$id_owner'
            and archived = '$archived_flag'
        ");
        $this->last_query = $database->get_last_query();
        $row = $database->fetch_object($res);
        
        return $row->count;
    }
    
    /**
     * @param string|array $id_or_ids
     *
     * @return int
     */
    public function mark_as_read($id_or_ids)
    {
        global $database;
        
        if( empty($id_or_ids) ) return 0;
        
        if( ! is_array($id_or_ids) ) $id_or_ids = array($id_or_ids);
        
        foreach($id_or_ids as &$id) $id = "'$id'";
        $id_or_ids = implode(", ", $id_or_ids);
        
        $date  = date("Y-m-d H:i:s");
        $query = "update pms set opened_date = '$date'
                  where opened_date = '0000-00-00 00:00:00'
                  and id_pm in ($id_or_ids)";
        $this->last_query = $query;
        
        return $database->exec($query);
    }
    
    public function delete_conversation_by_parnters($id_owner, $id_other)
    {
        global $database;
        
        $database->exec("
            delete from pm_conversations where
            id_owner = '$id_owner' and
            id_other = '$id_other'
        ");
        
        $database->exec("
            delete from pms where
            id_owner     = '$id_owner' and
            (
                (id_sender = '$id_owner' and id_recipient = '$id_other')
                or
                (id_sender = '$id_other' and id_recipient = '$id_owner')
            )
            
        ");
    }
    
    public function archive_conversation_by_partners($id_owner, $id_other)
    {
        global $database;
        
        $database->exec("
            update pm_conversations
            set archived = 1
            where id_owner = '$id_owner'
            and   id_other = '$id_other'
        ");
    }
    
    public function restore_archived_conversations($id_owner)
    {
        global $database;
        
        $database->exec("
            update pm_conversations set archived = '0' where id_owner = '$id_owner'
        ");
        $this->last_query = $database->get_last_query();
    }
}
