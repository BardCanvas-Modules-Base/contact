<?php
namespace hng2_modules\contact;

use hng2_repository\abstract_repository;

class pms_repository extends abstract_repository
{
    protected $row_class       = "hng2_base\\pm_record";
    protected $table_name      = "pms";
    protected $key_column_name = "id_pm";
    
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
            `id_sender`          = '{$obj->id_sender   }',
            `id_recipient`       = '{$obj->id_recipient}',
            `sent_date`          = '{$obj->sent_date   }',
            `contents`           = '{$obj->contents    }'
        ");
        $this->last_query = $database->get_last_query();
        
        $this->ping_conversation($record->id_sender,    $record->id_recipient, $record->sent_date);
        $this->ping_conversation($record->id_recipient, $record->id_sender,    $record->sent_date);
        
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
            "id_sender"    => $account->id_account,
            "id_recipient" => $id_recipient,
            "sent_date"    => date("Y-m-d H:i:s"),
            "contents"     => $content,
        ));
        $record->set_new_id();
        
        return $this->save($record);
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
        
        $id = uniqid();
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
     * @param $id_owner
     *
     * @return conversation_record[]
     * 
     * @throws \Exception
     */
    public function get_conversations($id_owner)
    {
        global $database;
        
        $res = $database->query("
            select * from pm_conversations where id_owner = '$id_owner'
            and archived = '0'
            order by last_event_date desc
        ");
        $this->last_query = $database->get_last_query();
        if( $database->num_rows($res) == 0 ) return array();
        
        $return = array();
        while($row = $database->fetch_object($res))
            $return[] = new conversation_record($row);
        
        return $return;
    }
}
