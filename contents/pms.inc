<?
/**
 * PMs browser
 *
 * @package    BardCanvas
 * @subpackage contact
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * $_GET params:
 * @param string "with" user_name of the conversation to highlight (with other user)
 */

use hng2_base\accounts_repository;
use hng2_modules\contact\pms_repository;

$accounts_repository = new accounts_repository();
$pms_repository      = new pms_repository();

$conversations = $pms_repository->get_conversations($account->id_account);

$author_ids = array();
foreach($conversations as $conversation) $author_ids[] = $conversation->id_other;
$authors = $accounts_repository->get_multiple($author_ids);
?>

<? if( count($conversations) == 0 ): ?>
    <div class="framed_content state_highlight" style="margin-top: 0;">
        <span class="fa fa-info-circle"></span>
        <?= $current_module->language->messages->no_conversations ?>
    </div>
    <? return; ?>
<? endif; ?>

<script type="text/javascript" src="<?=$config->full_root_path?>/lib/jquery.exptextarea.js"></script>
<script type="text/javascript">
    
    function open_conversation(other_user_id, trigger)
    {
        if( $(trigger).hasClass('selected') ) return;
        
        var $viewer = $('#pms_viewer');
        var $conversations = $viewer.find('.conversations');
    
        $conversations.find('.conversation_item').toggleClass('selected', false);
        $(trigger).toggleClass('selected', true);
        if( $conversations.hasClass('expanded') ) toggle_conversations_area();
        
        var $form = $('#pms_filter_form');
        $form.find('input[name="search_for"]').val('');
        $form.find('input[name="offset"]').val('0');
        $form.find('input[name="order"]').val('1');
        $form.find('input[name="with"]').val(other_user_id);
        $form.submit();
    }
    
    function reset_pms_filter()
    {
        var $form = $('#pms_filter_form');
        $form.find('input[name="search_for"]').val('');
        $form.find('select[name="limit"] option[value="20"]').prop('selected', true);
        $form.find('input[name="offset"]').val('0');
        $form.find('input[name="order"]').val('1');
    }
    
    //noinspection JSUnusedGlobalSymbols
    function paginate_pms_browser(value)
    {
        var $form = $('#pms_filter_form');
        $form.find('input[name="offset"]').val(value);
        $form.submit();
    }

    function add_pm_inlined_photo(type, icon)
    {
        var html = '<div class="attachment framed_content state_highlight clearfix">'
                 +     '<span class="fa pseudo_link fa-trash fa-fw pull-right" onclick="remove_pm_inlined_photo(this)"></span>'
                 +     sprintf('<span class="fa %s fa-fw"></span> ', icon)
                 +     sprintf('<input type="file" name="attachments[%1$s][]" accept="%1$s/*"> ', type)
                 + '</div>';
    
        $('#send_pm_paged_form').find('.attachments').append(html);
    }
    
    function remove_pm_inlined_photo(trigger)
    {
        $(trigger).closest('.attachment').fadeOut('fast', function() { $(this).remove(); });
    }
    
    function prepare_pm_sending(formData, $form, options)
    {
        $form.block(blockUI_medium_params);
    }
    
    function process_pm_response(responseText, statusText, xhr, $form)
    {
        if( responseText != 'OK' )
        {
            alert(responseText);
            
            $form.unblock();
            return;
        }
        
        $form.unblock();
        $('#refresh_pm_conversation').click();
    }
    
    function toggle_conversations_area()
    {
        var $target     = $('#pms_viewer').find('.conversations');
        var is_expanded = $target.hasClass('expanded');
        
        if( is_expanded )
        {
            $target.toggleClass('expanded', false);
            $target.find('.extender').removeClass('fa-times');
            $target.find('.extender').addClass('fa-caret-down');
            // $target.find('.conversation_item:not(.selected)').hide();
        }
        else
        {
            $target.toggleClass('expanded', true);
            $target.find('.extender').addClass('fa-times');
            $target.find('.extender').removeClass('fa-caret-down');
            // $target.find('.conversation_item').show();
        }
    }
</script>

<div id="pms_viewer" class="clearfix">
    
    <div class="conversations">
        <div class="extender fa fa-caret-down fa-fw fa-border" onclick="toggle_conversations_area()"></div>
        <? foreach($conversations as $index => $conversation):
            $author = $authors[$conversation->id_other];
            if( empty($_GET["with"]) ) $_GET["with"] = $author->user_name;
            $selected = $author->user_name == $_GET["with"] ? "selected" : "";
            ?>
            
            <div class="meta_box conversation_item <?= $selected ?> clearfix"
                 onclick="open_conversation('<?= $author->user_name ?>', this)">
                <img class="user_avatar" src="<?= $author->get_avatar_url() ?>">
                <div class="meta_section upper">
                    <span class="pull-right">
                        <?= get_minimized_date($conversation->last_event_date) ?>
                    </span>
                    <span class="meta_field">
                        <span class="fa fa-user"></span>
                        <?= $author->get_processed_display_name() ?>
                    </span>
                </div>
                <div class="meta_section">
                    <span class="meta_field dimmed">
                        Aquí va la última línea :D
                    </span>
                </div>
            </div>
            
        <? endforeach; ?>
    </div>
    
    <div class="messages">
        <? $url = "pms_browser.php?with=" . trim(stripslashes($_GET["with"])) . "&wasuuup=" . md5(mt_rand(1, 65535)); ?>
        <div id="pms_browser" class="ajax_record_browser" data-src="<?= $url ?>">
            <div class="framed_content state_highlight" style="padding: 100px 0; text-align: center; margin-top: 0;">
                <span class="fa fa-spinner fa-pulse"></span>
                <?= $language->wait ?>
            </div>
        </div>
        
    </div>
</div>

<div id="pm_paged_form_target" style="display: none;"></div>
