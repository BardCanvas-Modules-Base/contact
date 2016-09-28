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
$archived      = $pms_repository->get_conversations_count($account->id_account, 1);

$author_ids = array();
foreach($conversations as $conversation) $author_ids[] = $conversation->id_other;
$authors = $accounts_repository->get_multiple($author_ids);
?>

<script type="text/javascript">
    function restore_archived_conversations()
    {
        var url    = $_FULL_ROOT_PATH + '/contact/scripts/restore_archived_conversations.php';
        var params = {
            'wasuuup': wasuuup()
        };
        
        $.blockUI(blockUI_default_params);
        $.get(url, params, function(response)
        {
            if( response != 'OK' )
            {
                alert(response);
                $.unblockUI();
                return;
            }
            
            location.reload();
        });
    }
</script>

<? if( count($conversations) == 0 ): ?>
    <div class="framed_content state_highlight" style="margin-top: 0; padding: 20px;">
        <span class="fa fa-info-circle"></span>
        
        <?
        if( $archived == 0 )
            echo $current_module->language->messages->no_conversations;
        else
            echo replace_escaped_vars(
                $current_module->language->messages->no_conversations_restore_archive, '{$count}', $archived
            );
        ?>
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
    
    function delete_conversation(other_user_name, archive_only)
    {
        if( typeof archive_only == 'undefined' ) archive_only = false;
        
        if( ! archive_only )
        {
            var message = $('#delete_conversation_confirmation').text();
            if( ! confirm(message) ) return;
        }
        
        var url    = $_FULL_ROOT_PATH + '/contact/scripts/delete_conversation.php';
        var params = {
            'with':         other_user_name,
            'archive_only': archive_only ? 'true' : 'false',
            'wasuuup':      wasuuup()
        };
        
        var $viewport     = $('#pms_viewer');
        var $user_trigger = $viewport.find(sprintf(
            '.conversations .conversation_item[data-with="%s"]',
            other_user_name
        ));
        
        $viewport.block(blockUI_medium_params);
        $.get(url, params, function(response)
        {
            if( response != 'OK' )
            {
                alert(response);
                $viewport.unblock();
                return;
            }
            
            $viewport.unblock();
            var conversations_count = $viewport.find('.conversations .conversation_item').length - 1;
            if( conversations_count <= 0 )
            {
                top.location.reload();
            }
            else
            {
                var $next_user = $viewport.find(sprintf(
                    '.conversations .conversation_item:not([data-with="%s"]):visible:first',
                    other_user_name
                ));
                
                $user_trigger.fadeOut('fast', function() { $(this).remove(); });
                open_conversation($next_user.attr('data-with'), $next_user);
            }
        });
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
                 data-with="<?= $author->user_name ?>"
                 onclick="open_conversation('<?= $author->user_name ?>', this)">
                <img class="user_avatar" src="<?= $author->get_avatar_url() ?>">
                <div class="meta_section upper">
                    <span class="meta_field user_display_name" data-user-level="<?= $author->level ?>">
                        <span class="fa fa-user fa-fw"></span>
                        <?= $author->get_processed_display_name() ?>
                    </span>
                </div>
                <div class="meta_section">
                    <span class="meta_field dimmed">
                        <?= time_mini_string($conversation->last_event_date) ?>
                        (<?= time_elapsed_string($conversation->last_event_date) ?>)
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
<div id="delete_conversation_confirmation" style="display: none;"><?= unindent( $current_module->language->delete_conversation_confirmation ) ?></div>
