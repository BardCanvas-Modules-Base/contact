<?php
/**
 * PM sending form - called via AJAX
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
use hng2_modules\contact\pms_repository;

include "../config.php";
include "../includes/bootstrap.inc";
include "../lib/recaptcha-php-1.11/recaptchalib.php";

$accounts_repository = new accounts_repository();
$pms_repository = new pms_repository();

header("Content-Type: text/html; charset=utf-8");

if( ! $account->_exists ) throw_fake_401();
if( empty($_GET["target"]) ) die($current_module->language->messages->invalid_target);

$recipient = $accounts_repository->get($_GET["target"]);
if( is_null($recipient) ) die($current_module->language->messages->invalid_target);
?>

<form name="send_pm_form" id="send_pm_form" method="post" action="<?= $config->full_root_path ?>/contact/send_pm.php">
    <input type="hidden" name="target" value="<?= $recipient->user_name ?>">
    <textarea name="content" style="height: 125px; width: 100%;"
              placeholder="<?= $current_module->language->send_pm_form->content->placeholder ?>"></textarea>
</form>
