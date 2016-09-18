<?php
namespace hng2_modules\contact;

use hng2_repository\abstract_record;

class conversation_record extends abstract_record
{
    public $id_conversation;
    public $id_owner;
    public $id_other;
    public $last_event_date;
    public $archived;
    
    public function set_new_id()
    {
        list($sec, $usec) = explode(".", microtime(true));
        $this->id_conversation = "1052" . $sec . sprintf("%05.0f", $usec) . mt_rand(1000, 9999);;
    }
}
