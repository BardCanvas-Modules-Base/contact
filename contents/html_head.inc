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
        
        if( parseInt($counter.text()) != unread_pms )
            update_pms_counter(unread_pms);
    }
</script>
