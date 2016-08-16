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
 */

use hng2_base\accounts_repository;
use hng2_modules\contact\pm_record;
use hng2_modules\contact\pms_repository;

include "../config.php";
include "../includes/bootstrap.inc";
include "../lib/recaptcha-php-1.11/recaptchalib.php";

$accounts_repository = new accounts_repository();
$pms_repository = new pms_repository();

header("Content-Type: text/html; charset=utf-8");

if( ! $account->_exists ) throw_fake_401();
if( empty($_POST["target"]) ) die($current_module->language->messages->invalid_target);

$recipient = $accounts_repository->get($_POST["target"]);
if( is_null($recipient) ) die($current_module->language->messages->invalid_target);

$record = new pm_record(array(
    "id_sender"    => $account->id_account,
    "id_recipient" => $recipient->id_account,
    "contents"     => trim(stripslashes($_POST["content"]))
));
$record->set_new_id();
$pms_repository->save($record);

send_notification($account->id_account, "success", replace_escaped_vars(
    $current_module->language->messages->pm_sent_ok,
    '{$recipient}',
    $recipient->display_name
));

if( $recipient->is_online() )
{
    $wasuuup = md5(mt_rand(1, 65535));
    $link    = "{$config->full_root_path}/contact/pms.php?with={$account->user_name}&wasuuup={$wasuuup}";
    send_notification($recipient->id_account, "information", replace_escaped_vars(
        $current_module->language->messages->received_pm,
        array('{$sender}', '{$link}'),
        array($account->display_name, $link)
    ));
}

echo "OK";
