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
    
    public function get_processed_contents()
    {
        global $config, $modules;
        
        $contents = nl2br($this->contents);
        $contents = convert_emojis($contents);
        
        $contents = convert_media_tags($contents);
        $contents = autolink_hash_tags($contents, "{$config->full_root_path}/tag/");
        
        $config->globals["processing_contents"] = $contents;
        $modules["contact"]->load_extensions("pm_record_class", "get_processed_contents");
        $contents = $config->globals["processing_contents"];
        
        return $contents;
    }
}
