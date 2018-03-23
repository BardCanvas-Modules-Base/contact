<?php
/**
 * Contact form
 *
 * @package    HNG2
 * @subpackage contact
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * @var module $current_module
 */

use hng2_base\accounts_repository;
use hng2_base\config;
use hng2_base\module;
use hng2_modules\mobile_controller\toolbox;

include "../config.php";
include "../includes/bootstrap.inc";
session_start();

if( $modules["mobile_controller"]->enabled && ! empty($_REQUEST["bcm_platform"]) )
{
    $toolbox = new toolbox();
    $toolbox->output_type = "HTML";
    $toolbox->open_session();
}

if( ! $account->_exists && $settings->get("modules:contact.csrf_for_guests") == "true" )
    if( empty($_SESSION["{$config->website_key}_contact_form_token"]) )
        die(unindent($current_module->language->messages->missing_posting_token));

$accounts_repository = new accounts_repository();

header("Content-Type: text/plain; charset=utf-8");

if( ! $account->_exists )
{
    if( empty($_POST["name"]) )  die($current_module->language->messages->empty_name);
    if( empty($_POST["email"]) ) die($current_module->language->messages->empty_email);
    
    if( filter_var($_POST["email"], FILTER_VALIDATE_EMAIL) === false )
        die($current_module->language->messages->invalid_mail);
    
    if( $settings->get("engine.recaptcha_private_key") != "" )
    {
        if( ! isset($_POST['g-recaptcha-response']) )
            die($current_module->language->messages->invalid_captcha);
        
        $cap = trim(stripslashes($_POST['g-recaptcha-response']));
        $ch  = curl_init("https://www.google.com/recaptcha/api/siteverify?secret={$settings->get("engine.recaptcha_private_key")}&response={$cap}");
        curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "GET");
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        $res = curl_exec($ch);
        
        if( curl_error($ch) )
            die(replace_escaped_objects(
                $current_module->language->messages->captcha_api_error,
                array('{$error}' => curl_error($ch))
            ));
        
        $obj = json_decode($res);
        if( empty($obj) )
            die(replace_escaped_objects(
                $current_module->language->messages->captcha_api_error,
                array('{$error}' => print_r($res))
            ));
        
        if( ! $obj->success )
            die($current_module->language->messages->invalid_captcha);
        
        curl_close($ch);
    }
}

if( empty($_POST["subject"]) ) die($current_module->language->messages->empty_subject);
if( empty($_POST["content"]) ) die($current_module->language->messages->empty_content);

$subject    = trim(stripslashes($_POST["subject"]));
$recipients = array( $settings->get("engine.website_name") => $settings->get("engine.webmaster_address") );
$sender     = $account->_exists
            ? array( $account->display_name => $account->email )
            : array( stripslashes($_POST["name"]) => stripslashes($_POST["email"]) );

$ip       = get_user_ip();
$location = forge_geoip_location( $ip );
$referer  = $_SERVER["HTTP_REFERER"];

if( ! empty($_POST["target"]) )
{
    $target = $accounts_repository->get($_POST["target"]);
    if( is_null($target) ) die( $current_module->language->messages->invalid_target );
    
    if( $account->id_account == $target->id_account )
        die( $current_module->language->messages->self_messages_not_allowed );
    
    if( $target->get_engine_pref("@contact:allow_emails") == "false" && $account->level < config::MODERATOR_USER_LEVEL )
        die( $current_module->language->messages->user_cannot_be_emailed );
    
    $recipients = array($target->display_name => $target->email);
    if( ! empty($target->alt_email) ) $recipients["{$target->display_name} (2)"] = $target->alt_email;
    
    if( $target->level >= config::AUTHOR_USER_LEVEL ) $ip = $location = $referer = "N/A";
}

$config->globals["@contact:sender"] = $sender;
$current_module->load_extensions("send_email", "pre_send");
$sender = $config->globals["@contact:sender"];
unset( $config->globals["@contact:sender"] );

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
        '{$referer}',
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
        $ip,
        $location,
        $referer,
    )
);

$config->globals["@contact:proceed_with_send"] = true;
$current_module->load_extensions("send_email", "pre_actual_send");

if( $config->globals["@contact:proceed_with_send"] )
{
    $res = send_mail($subject, $body, $recipients, $sender);
    
    if( $res != "OK" ) echo $res;
    else               echo $_REQUEST["stop_on_success"] == "true" ?
                            "OK:{$current_module->language->messages->sent_close}" :
                            "OK:{$current_module->language->messages->sent_ok}";
}

$current_module->load_extensions("send_email", "post_send");

if( ! empty($_SESSION["{$config->website_key}_contact_form_token"]) )
    unset( $_SESSION["{$config->website_key}_contact_form_token"] );
