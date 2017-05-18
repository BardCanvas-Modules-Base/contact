<?php
/**
 * Contact form
 *
 * @package    HNG2
 * @subpackage contact
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * Trailing vars:
 * @var accounts_repository $accounts_repository
 * 
 * $_GET params:
 * @param "target" user_name of target account
 */

use hng2_base\accounts_repository;
use hng2_base\config;

$sender_name  = $account->_exists ? htmlspecialchars($account->display_name) : "";
$sender_email = $account->_exists ? $account->email                          : "";
$readonly     = $account->_exists ? "readonly"                               : "";
$target       = empty($_GET["target"]) ? null : $accounts_repository->get($_GET["target"]);

$config->globals["@contact:sender_name"]  = $sender_name;
$config->globals["@contact:sender_email"] = $sender_email;
$config->globals["@contact:subject"]     = "";
$config->globals["@contact:content"]     = "";
$config->globals["@contact:show_errors"] = array();
$current_module->load_extensions("form", "presets");
$sender_name  = $config->globals["@contact:sender_name"];
$sender_email = $config->globals["@contact:sender_email"];
unset( $config->globals["@contact:sender_name"], $config->globals["@contact:sender_email"] );
?>

<script type="text/javascript" src="<?= $config->full_root_path ?>/lib/jquery.exptextarea.js"></script>

<h1><?= $template->get("title") ?></h1>

<? if( count($config->globals["@contact:show_errors"]) > 0 ): ?>
    <div class="framed_content state_ko">
        <? foreach($config->globals["@contact:show_errors"] as $error): ?>
            <p><span class="fa fa-warning"></span> <?= $error ?></p>
        <? endforeach; ?>
    </div>
<? endif; ?>

<? if( empty($_GET["target"]) && $settings->get("engine.webmaster_address") == "" ): ?>
    <div class="framed_content state_ko">
        <span class="fa fa-warning"></span>
        <?= $language->errors->webmaster_address_empty ?>
    </div>
    <? return; ?>
<? endif; ?>

<? if( ! empty($_GET["target"]) && is_null($target) ): ?>
    <div class="framed_content state_ko">
        <span class="fa fa-warning"></span>
        <?= $current_module->language->messages->invalid_target ?>
    </div>
    <? return; ?>
<? endif; ?>

<? if( ! is_null($target)
       && $settings->get("modules:contact.no_anonymous_emails") == "true"
       && ! $account->_exists ): ?>
    <div class="framed_content state_ko">
        <span class="fa fa-warning"></span>
        <?= $current_module->language->messages->guests_cant_email ?>
    </div>
    <? return; ?>
<? endif; ?>

<? if( ! is_null($target)
       && $target->get_engine_pref("@contact:allow_emails") == "false"
       && $account->level < config::MODERATOR_USER_LEVEL ): ?>
    <div class="framed_content state_ko">
        <span class="fa fa-warning"></span>
        <?= $current_module->language->messages->user_cannot_be_emailed ?>
    </div>
    <? return; ?>
<? endif; ?>

<div id="contact_form_target" style="display: none;"></div>

<form name="contact_form" id="contact_form" method="post" action="<?= $config->full_root_path ?>/contact/send_email.php">
    
    <input type="hidden" name="action" value="send">
    
    <? if( ! is_null($target) ): ?>
        <div class="field">
            <div class="caption">
                <?= $current_module->language->fields->target ?>
            </div>
            <div class="input">
                <input type="hidden" name="target" value="<?= $target->id_account ?>">
                <input type="text" readonly value="<?= htmlspecialchars($target->display_name) ?>">
            </div>
        </div>
    <? endif; ?>
    
    <div class="field">
        <div class="caption">
            <?= $current_module->language->fields->name ?>
        </div>
        <div class="input">
            <input type="text" <?= $readonly ?> name="name" value="<?= $sender_name ?>">
        </div>
    </div>
    
    <div class="field">
        <div class="caption">
            <?= $current_module->language->fields->email->caption ?>
        </div>
        <div class="input">
            <input type="text" <?= $readonly ?> name="email" value="<?= $sender_email ?>">
        </div>
        <? if( ! $account->_exists ): ?>
            <? if( empty($_GET["target"]) ): ?>
                <div class="input info_handler"><?= $current_module->language->fields->email->info->general ?></div>
            <? else: ?>
                <div class="input info_handler"><?= $current_module->language->fields->email->info->targeted ?></div>
            <? endif; ?>
        <? endif; ?>
    </div>
    
    <div class="field">
        <div class="caption">
            <?= $current_module->language->fields->subject ?>
        </div>
        <div class="input">
            <textarea name="subject" class="expandible_textarea"><?= htmlspecialchars($config->globals["@contact:subject"]) ?></textarea>
        </div>
    </div>
    
    <div class="field">
        <div class="caption">
            <?= $current_module->language->fields->content ?>
        </div>
        <div class="input">
            <textarea name="content" class="tinymce_minimal"><?= htmlspecialchars($config->globals["@contact:content"]) ?></textarea>
        </div>
    </div>
    
    <? if( ! $account->_exists && $settings->get("engine.recaptcha_public_key") != "" ): ?>
        <div class="field">
            <?= recaptcha_get_html($settings->get("engine.recaptcha_public_key"), null, $_SERVER["HTTPS"]) ?>
        </div>
    <? endif; ?>
    
    <p>
        <button type="submit">
            <?= $language->submit ?>
            <span class="fa fa-play"></span>
        </button>
    </p>
    
</form>

<script type="text/javascript">
    
    function prepare_contact_form_serialization($form)
    {
        $form.find('textarea[class*="tinymce"]').each(function()
        {
            var id      = $(this).attr('id');
            var editor  = tinymce.get(id);
            var content = editor.getContent();
            $(this).val( content );
        });
    }
    
    function prepare_contact_form_submission(data, $form)
    {
        $form.block(blockUI_default_params);
    }
    
    function process_contact_form_submission(response, status, xhr, $form)
    {
        if( response.indexOf('OK:') < 0 )
        {
            alert( response );
            $form.unblock();
            
            if( Recaptcha ) Recaptcha.reload();
            return;
        }
        
        message = response.replace('OK:', '');
        alert(message);
        location.reload();
    }
    
    $(document).ready(function()
    {
        $('#contact_form').ajaxForm({
            target:          '#contact_form_target',
            beforeSerialize: prepare_contact_form_serialization,
            beforeSubmit:    prepare_contact_form_submission,
            success:         process_contact_form_submission
        });
        
        $('.expandible_textarea').expandingTextArea();
    })
</script>
