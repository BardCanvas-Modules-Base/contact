
function send_pm(trigger, target_id_account, dialog_title)
{
    var url = $_FULL_ROOT_PATH
        + '/contact/render_pm_form.php'
        + '?target='  + target_id_account
        + '&wasuuup=' + parseInt(Math.random() * 1000000000000000);
    
    $(trigger).block(blockUI_smallest_params);
    $.get(url, function(response)
    {
        $(trigger).unblock();
        
        var html = '<div id="temp_pm_dialog" style="display: none;">'
            + '<div id="temp_pm_target" style="display: none;"></div>'
            + response
            + '</div>';
        
        $('body').append(html);
        $('#send_pm_form').ajaxForm({
            target:       '#temp_pm_target',
            beforeSubmit: prepare_pm_submission,
            success:      process_pm_response
        });
        
        var width = $(window).width() - 20;
        if( width < 200 ) width = 200;
        if( width > 400 ) width = 400 - 20;
        var height = 275;
        
        $('#temp_pm_dialog').dialog({
            modal:  true,
            width:  width,
            height: height,
            title:  dialog_title,
            close:  function() { $(this).dialog('destroy').remove(); },
            buttons: [
                {
                    text:  cancel_caption,
                    icons: { primary: "ui-icon-cancel" },
                    click: function() { $(this).dialog( "close" ); }
                },
                {
                    text: submit_caption,
                    icons: { primary: "ui-icon-check" },
                    click: function() { $('#send_pm_form').submit(); }
                }
            ]
        })
    });
}

function prepare_pm_submission()
{
    $('#temp_pm_dialog').block(blockUI_medium_params);
}

function process_pm_response(response)
{
    var $dialog = $('#temp_pm_dialog');
    $dialog.unblock();
    
    if( response != 'OK' )
    {
        alert(response);
        
        return;
    }
    
    $dialog.dialog('destroy').remove();
}

/**
 * @param {int} unread_pms
 */
function update_pms_counter(unread_pms)
{
    var $trigger = $('#unread_pms_count');
    var $counter = $trigger.find('.unread_count');
    
    $counter.text(unread_pms);
    
    if(unread_pms == 0) $trigger.toggleClass('alerted', false);
    else                $trigger.toggleClass('alerted', true);
}
