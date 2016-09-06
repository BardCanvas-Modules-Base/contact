<?php
/**
 * Email blacklist add/remove
 *
 * @package    HNG2
 * @subpackage contact
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *             
 * $_GET params:
 * @param string "address" ENCRYPTED email address to add/remove
 * @param bool   "remove"  If "true", address will be removed instead of added
 */

use hng2_base\config;
use hng2_modules\contact\toolbox;
use hng2_modules\security\email_blacklist_record;
use hng2_modules\security\emails_blacklist_repository;

include "../../config.php";
include "../../includes/bootstrap.inc";

$template->set_page_title($current_module->language->email_blacklisting->title);
$template->set("blacklist_popup_title", $current_module->language->email_blacklisting->title);
$template->page_contents_include = "blacklist_popup.inc";

if( ! $modules["security"]->enabled )
{
    $template->set("blacklist_popup_content", $current_module->language->email_blacklisting->security_module_missing);
    include "{$template->abspath}/popup.php";
    
    die();
}

if( empty($_GET["address"]) )
{
    $template->set("blacklist_popup_content", $current_module->language->email_blacklisting->missing_address);
    include "{$template->abspath}/popup.php";
    
    die();
}

$address = decrypt($_GET["address"], $config->encryption_key);
if( ! filter_var($address, FILTER_VALIDATE_EMAIL) )
{
    $message = replace_escaped_vars(
        $current_module->language->email_blacklisting->invalid_address, '{$address}', htmlspecialchars($address)
    );
    $template->set("blacklist_popup_content", $message);
    include "{$template->abspath}/popup.php";
    
    die();
}

$toolbox    = new toolbox();
$repository = new emails_blacklist_repository();

if( $_GET["remove"] != "true" ) # <--- Add
{
    $res = $repository->save(new email_blacklist_record(array("email" => $address)));
    if( $res > 0 )
    {
        $toolbox->notify_mods_on_address_blacklisting($address);
        
        $template->set("blacklist_popup_content", replace_escaped_vars(
            $current_module->language->email_blacklisting->blacklisted_ok, '{$address}', htmlspecialchars($address)
        ));
    }
    else
    {
        $template->set("blacklist_popup_content", replace_escaped_vars(
            $current_module->language->email_blacklisting->already_blacklisted, '{$address}', htmlspecialchars($address)
        ));
    }
}
else # <--- Remove, only for mods
{
    if( $account->level < config::MODERATOR_USER_LEVEL )
    {
        $template->set("blacklist_popup_content", $current_module->language->email_blacklisting->only_for_mods);
        include "{$template->abspath}/popup.php";
        
        die();
    }
    
    $res = $repository->delete($address);
    if( $res > 0 )
    {
        broadcast_to_moderators("information", replace_escaped_vars(
            $current_module->language->email_blacklisting->notification_to_mods->on_removal->body,
            array('{$current_user_display_name}', '{$blacklisted_address}'),
            array($account->get_processed_display_name(), $address)
        ));
        
        $template->set("blacklist_popup_content", replace_escaped_vars(
            $current_module->language->email_blacklisting->removed_ok, '{$address}', htmlspecialchars($address)
        ));
    }
    else
    {
        $template->set("blacklist_popup_content", replace_escaped_vars(
            $current_module->language->email_blacklisting->not_found, '{$address}', htmlspecialchars($address)
        ));
    }
}

include "{$template->abspath}/popup.php";
