<?php
namespace hng2_modules\contact;

use hng2_repository\abstract_record;

class pm_record extends abstract_record
{
    public $id_pm;
    public $id_owner;
    public $id_sender;
    public $id_recipient;
    public $sent_date;
    public $opened_date;
    public $sender_archived;
    public $recipient_archived;
    public $contents;
    
    public function set_new_id()
    {
        $this->id_pm = uniqid();
    }
}
