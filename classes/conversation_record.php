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
        $this->id_conversation = make_unique_id("21");
    }
}
