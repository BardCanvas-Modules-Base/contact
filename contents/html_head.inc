<?
/**
 * Contact header functions
 *
 * @package    BardCanvas
 * @subpackage contact
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */
?>

<!-- Contact html head -->

<script type="text/javascript">
    var submit_caption = '<?= $language->submit ?>';
    var cancel_caption = '<?= $language->cancel ?>';
    
    var _PM_DIALOG_TITLE_TEMPLATE = '<?= $language->contact->pm->send_to ?>';
</script>

<script type="text/javascript" src="<?= $config->full_root_path ?>/contact/media/functions~v<?=$config->scripts_version?>.js"></script>
<link rel="stylesheet" type="text/css" href="<?= $config->full_root_path ?>/contact/media/styles~v<?=$config->scripts_version?>.css">

<!-- Hooks for the notifications getter: actions for prepared data -->
<script type="text/javascript"
        data-hooks-to="notifications-getter"
        data-notifications-getter-hooking-function="update_unread_pms_amount_hooker">
    
    /**
     * @return {function}
     */
    function update_unread_pms_amount_hooker()
    {
        return update_unread_pms_amount;
    }
    
    /**
     * @param {object} data
     */
    function update_unread_pms_amount(data)
    {
        if( typeof data == 'undefined' ) return;
        if( typeof data.unread_pms == 'undefined' ) return;
        
        var unread_pms = parseInt(data.unread_pms);
        if( isNaN(unread_pms) ) unread_pms = 0;
    
        var $trigger = $('#unread_pms_count');
        var $counter = $trigger.find('.unread_count');
        
        var previous_count = parseInt($counter.text());
        if( previous_count != unread_pms )
        {
            update_pms_counter(unread_pms);
            if( unread_pms > 0 ) play_notification_sound('message');
        }
    }
</script>

<!-- Hooks for the notifications getter: actions for prepared data -->
<script type="text/javascript"
        data-hooks-to="notifications-getter"
        data-notifications-getter-hooking-function="update_online_users_list_hooker">
    
    /**
     * @return {function}
     */
    function update_online_users_list_hooker()
    {
        return update_online_users_list;
    }
    
    /**
     * @param {object} data
     */
    function update_online_users_list(data)
    {
        var list = [];
        
        if( typeof data != 'undefined' )
            if( typeof data.online_users != 'undefined' )
                list = data.online_users;
        
        var $target  = $('#online_users_list');
        var $trigger = $target.find('.trigger');
        var $content = $target.find('.content');
        
        $trigger.find('.users_count').text(list.length);
        
        if(list.length == 0)
        {
            $content.html('');
            $target.attr('data-state', 'collapsed');
            $target.hide();
            
            return;
        }
        
        var avatar = '';
        var html = '';
        for(var i in list)
        {
            var item = list[i];
            
            var div 
                = '<div class="user" onclick="send_pm(this, \'%s\', \'%s\')" style="%s">'
                +     '<span class="user_display_name" data-user-level="%s">'
                +         '<span class="name">%s</span>'
                +         '<span class="fa fa-user pull-right"></span>'
                +     '</a>'
                + '</div>'
                ;
            
            avatar_markup = item.avatar == '' ? '' : 'background-image: url(\'' + item.avatar + '\')';
            html = html + sprintf(
                    div,
                    item.id_account, item.display_name, avatar_markup,
                    item.level,
                    item.display_name
                )
            ;
        }
        
        var actual_html = $content.data('actual_html');
        if( actual_html == html ) return;
        
        $content.html(html);
        $content.data('actual_html', html);
        if( $target.not(':visible') ) $target.show('slide', {direction: 'down'});
    }
    
    function toggle_online_users_list()
    {
        var $target  = $('#online_users_list');
        var $content = $target.find('.content');
        
        if( $target.attr('data-state') == 'expanded' )
        {
            set_engine_pref('@contact:online_users_list_state', 'collapsed');
            $target.attr('data-state', 'collapsed');
        }
        else
        {
            set_engine_pref('@contact:online_users_list_state', 'expanded');
            $target.attr('data-state', 'expanded');
        }
    }
</script>
