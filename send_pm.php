<?php
/**
 * PM sender
 *
 * @package    HNG2
 * @subpackage contact
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * $_GET params:
 * @param string "target"  Id or user name
 * @param string "content" Message per-se
 * @param string "attachments" Attached images (optinal)
 * @param bool   "no_sender_notification" (optinal)
 */

use hng2_base\accounts_repository;
use hng2_modules\contact\pms_repository;

include "../config.php";
include "../includes/bootstrap.inc";
include "../lib/recaptcha-php-1.11/recaptchalib.php";

$accounts_repository = new accounts_repository();
$pms_repository = new pms_repository();

header("Content-Type: text/html; charset=utf-8");

if( ! $account->_exists ) throw_fake_401();
if( empty($_POST["target"]) ) die($current_module->language->messages->invalid_target);
if( empty($_POST["content"]) && empty($_FILES["attachments"]) ) die($current_module->language->messages->content);

$recipient = $accounts_repository->get($_POST["target"]);
if( is_null($recipient) ) die($current_module->language->messages->invalid_target);

$contents = htmlspecialchars(trim(stripslashes($_POST["content"])));

if( ! empty($_FILES["attachments"]) )
{
    $uploads = array();
    
    foreach($_FILES["attachments"] as $field => $types)
        foreach($types as $type => $entries)
            foreach($entries as $index => $value)
                $uploads[$type][$index][$field] = $value;
    
    $target_dir = "{$config->datafiles_location}/pm_attachments/{$account->user_name}";
    if( ! is_dir($target_dir) )
    {
        if( ! @mkdir($target_dir, 0777, true) )
            die(replace_escaped_vars($current_module->language->messages->cannot_create_dir, '{$dir}', $target_dir));
        
        @chmod($target_dir, 0777);
    }
    
    foreach($uploads["image"] as $upload)
    {
        if( ! is_uploaded_file($upload["tmp_name"]) )
            die(replace_escaped_vars(
                $current_module->language->messages->invalid_uploaded_file, '{$file}', $upload["tmp_name"]
            ));
        
        $parts       = explode(".", $upload["name"]);
        $extension   = strtolower(array_pop($parts));
        $name        = wp_sanitize_filename(implode(".", $parts));
        $target_file = "$target_dir/$name.$extension";
        $date        = date("Ymd-His");
        
        if( ! in_array($extension, array("png", "jpg", "jpeg", "gif")) )
            die($current_module->language->messages->invalid_pm_attachment);
        
        if( file_exists($target_file) )
            $target_file = "$target_dir/$name-$date.$extension";
        
        if( ! @move_uploaded_file($upload["tmp_name"], $target_file) )
            die(replace_escaped_vars(
                $current_module->language->messages->cannot_move_attachment,
                array('{$file}', '{$target}'),
                array($upload["name"], $target_file)
            ));
        
        $target_url = "{$config->full_root_path}/pm_attachments/{$account->user_name}/" . basename($target_file);
        $contents .= "\n\n<img class='pm_attachment' src='$target_url'>";
    }
    
    $contents = trim($contents);
}

$pms_repository->send($recipient->id_account, $contents);

if( $_POST["no_sender_notification"] != "true" )
    send_notification($account->id_account, "success", replace_escaped_vars(
        $current_module->language->messages->pm_sent_ok,
        '{$recipient}',
        $recipient->display_name
    ));

$wasuuup = md5(mt_rand(1, 65535));
$link    = "{$config->full_root_path}/contact/pms.php?with={$account->user_name}&wasuuup={$wasuuup}";
send_notification($recipient->id_account, "information", replace_escaped_vars(
    $current_module->language->messages->received_pm,
    array('{$sender}', '{$link}'),
    array($account->display_name, $link)
));

echo "OK";
