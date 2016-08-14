<?php
/**
 * Contact form
 *
 * @package    HNG2
 * @subpackage contact
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */

use hng2_base\accounts_repository;

include "../config.php";
include "../includes/bootstrap.inc";
include "../lib/recaptcha-php-1.11/recaptchalib.php";

$accounts_repository = new accounts_repository();

header("Content-Type: text/plain; charset=utf-8");

if( ! $account->_exists )
{
    if( empty($_POST["name"]) )  die($current_module->language->messages->empty_name);
    if( empty($_POST["email"]) ) die($current_module->language->messages->empty_email);
    
    if( filter_var($_POST["email"], FILTER_VALIDATE_EMAIL) === false )
        die($current_module->language->messages->invalid_email);
    
    $res = recaptcha_check_answer($settings->get("engine.recaptcha_private_key"), get_remote_address(), $_POST["recaptcha_challenge_field"], $_POST["recaptcha_response_field"]);
    if( ! $res->is_valid ) die($current_module->language->messages->invalid_captcha);
}

if( empty($_POST["subject"]) ) die($current_module->language->messages->empty_subject);
if( empty($_POST["content"]) ) die($current_module->language->messages->empty_content);

$subject    = trim(stripslashes($_POST["subject"]));
$recipients = array( $settings->get("engine.website_name") => $settings->get("engine.webmaster_address") );
$sender     = $account->_exists
            ? array( $account->display_name => $account->email )
            : array( stripslashes($_POST["name"]) => stripslashes($_POST["email"]) );

if( ! empty($_POST["target"]) )
{
    $target = $accounts_repository->get($_POST["target"]);
    if( is_null($target) ) die( $current_module->language->messages->invalid_target );
    
    if( $account->id_account == $target->id_account )
        die( $current_module->language->messages->self_messages_not_allowed );
    
    if( $target->get_engine_pref("@contact:disable_user_emails", "false") == "true" )
        die( $current_module->language->messages->user_cannot_be_emailed );
    
    $recipients = array($target->display_name => $target->email);
    if( ! empty($target->alt_email) ) $recipients["{$target->display_name} (2)"] = $target->alt_email;
}

$body = replace_escaped_vars(
    $current_module->language->body,
    array(
        '{$recipient_name}',
        '{$sender_email}',
        '{$sender_name}',
        '{$website_name}',
        '{$body}',
        '{$recipient_email}',
        '{$mailer_name}',
        '{$website_name}',
        '{$date}',
        '{$origin_host}',
        '{$origin_location}',
    ),
    array(
        key($recipients),
        current($sender),
        key($sender),
        $settings->get("engine.website_name"),
        trim(stripslashes($_POST["content"])),
        current($recipients),
        $settings->get("engine.mail_sender_name"),
        $settings->get("engine.website_name"),
        date("Y-m-d H:i:s"),
        get_user_ip(),
        forge_geoip_location( get_user_ip() ),
    )
);

$res = send_mail($subject, $body, $recipients, $sender);

if( $res != "OK" ) echo $res;
else               echo "OK:{$current_module->language->messages->sent_ok}";
