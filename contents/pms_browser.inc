<?
/**
 * PMs browser
 *
 * @package    BardCanvas
 * @subpackage contact
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var config $config
 */

use hng2_base\account_record;
use hng2_base\accounts_repository;
use hng2_base\config;
use hng2_modules\contact\pm_record;
use hng2_modules\contact\pms_repository;
use hng2_tools\record_browser;

$accounts_repository = new accounts_repository();
$other_account       = $accounts_repository->get($_GET["with"]);
if( is_null($other_account) ) $other_account = new account_record();

$browser    = new record_browser("{$config->website_key}_{$current_module->name}_pms_browser");
$repository = new pms_repository();

/**
 * @var int    $offset
 * @var int    $limit
 * @var int    $order
 * @var string $search_state
 * @var string $search_for
 */
#region Nav filters
$built_vars = $browser->build_vars(20, 1);
foreach($built_vars as $key => $val) $$key = $val;
# echo "<pre>\$built_vars := " . print_r($built_vars, true) . "</pre>";
$where = array(
    "(id_owner = '{$account->id_account}' and (
        (id_sender = '{$account->id_account}' and id_recipient = '{$other_account->id_account}')
        or
        (id_sender = '{$other_account->id_account}' and id_recipient = '{$account->id_account}')
    ))"
);

$search_for = empty($search_for) ? "" : trim($search_for);
if( ! empty($search_for) )
{
    if( strpos($search_for, ",") !== false )
    {
        $search_for_exploded = explode(",", $search_for);
        $where_line          = "";
        foreach( $search_for_exploded as $this_term )
        {
            $this_term = trim($this_term);
            if( ! empty($this_term) )
                $where_line .= "contents like '%{$this_term}%' or ";
        }
        $where[] = substr($where_line, 0, -4);
    }
    else
    {
        $search_for = trim($search_for);
        $where[] = "contentd like '%{$search_for}%'";
    }
}
#endregion

#region Nav pointers
$record_count = $repository->get_record_count($where);
$pagination   = $browser->build_pagination($record_count, $limit, $offset);
#endregion

#region Data grabbing
$sqlorder = "sent_date desc";
$records  = $repository->find($where, $limit, $offset, $sqlorder);
#endregion

$style = "";
if( empty($_GET["with"]) )         $style = "display: none;";
else if( is_null($other_account) ) $style = "display: none;";
?>

<h2 class="clearfix" style="<?= $style ?>">
    <button id="refresh_pm_conversation" class="pull-right" onclick="paginate_pms_browser(<?= $offset ?>)">
        <span class="fa fa-refresh"></span>
        <?= $language->refresh ?>
    </button>
    
    <?= replace_escaped_vars($current_module->language->pms_nav->title, '{$with}', $other_account->get_processed_display_name()) ?>
    
    <a href="#send_pm_inlined_form">
        <span class="fa fa-paper-plane"></span>
        <?= count($records) == 0 
            ? $current_module->language->pms_nav->actions->send_pm 
            : $current_module->language->pms_nav->actions->send_reply ?>
    </a>
</h2>

<div class="filtering clearfix" style="<?= $style ?>">
    <form name="pms_filter_form" id="pms_filter_form" action="<?= $_SERVER["PHP_SELF"] ?>?wasuuup=<? echo mt_rand(1, 65535); ?>"
          method="get">
        
        <input type="hidden" name="mode"   value="set_filter">
        <input type="hidden" name="order"  value="<?= $order ?>">
        <input type="hidden" name="with"   value="<?= trim(stripslashes($_GET["with"])) ?>">
        <input type="hidden" name="offset" value="0">
        <input type="hidden" name="previous_offset" value="<?= $offset ?>">
        
        <span style="float: right;">
            <?# = $browser->get_pagination_button("next", "paginate_pms_browser", $pagination, "fa-caret-left") ?>
            <?= $language->record_nav->page ?>
            <? $page = $pagination["total_pages"] + 1 - $pagination["this_page_number"]; ?>
            <?= $page < 0 ? 0 : $page ?>/<?= $pagination["total_pages"]; ?>
            (<?= $pagination["total_records"]; ?> <?= $language->record_nav->entries ?>)
            <?# = $browser->get_pagination_button("previous", "paginate_pms_browser", $pagination, "fa-caret-right") ?>
        </span>
        
        <?= $language->record_nav->search ?>
        <input type="text" name="search_for"
               value="<?= htmlspecialchars($search_for) ?>" size="29"
               placeholder="<?= $current_module->language->pms_nav->search_placeholder ?>">
        
        <br>
        <?= $language->record_nav->show ?>
        <select name="limit">
            <? foreach(array(5, 10, 20, 30, 40, 50, 100, 200, 300, 400, 500) as $recs): ?>
                <option <? if($limit == $recs) echo "selected" ?> value="<?= $recs ?>"><?= $recs ?></option>
            <? endforeach; ?>
        </select>
        <?= $language->record_nav->recs_per_page ?>
        
        <button type="submit"><?= $language->record_nav->buttons->apply ?></button>
        <button type="submit" onclick="reset_pms_filter();"><?= $language->record_nav->buttons->reset ?></button>
    </form>
</div>

<? if( empty($_GET["with"]) ): ?>
    <div class="framed_content state_ko" style="margin-top: 0;">
        <span class="fa fa-info-circle"></span>
        <?= $current_module->language->pms_nav->select_conversation ?>
    </div>
    <? return; ?>
<? endif;  ?>

<? if( is_null($other_account) ): ?>
    <div class="framed_content state_ko" style="margin-top: 0;">
        <span class="fa fa-info-circle"></span>
        <?= $current_module->language->pms_nav->other_account_not_found ?>
    </div>
    <? return; ?>
<? endif;  ?>

<?
$accounts = array(
    $account->id_account       => $account,
    $other_account->id_account => $other_account,
);

/** @var pm_record[] $records */
$records = array_reverse($records);
$unread_ids = array();
?>
<div id="conversation_container">
    
    <div class="pagination_button top">
        <?= $browser->get_pagination_button(
            "next", "paginate_pms_browser", $pagination, "fa-caret-up"
        ) ?>
    </div>
    
    <? if( $record_count == 0 ): ?>
        
        <div class="framed_content state_ko">
            <span class="fa fa-info-circle"></span>
            <?= $current_module->language->pms_nav->no_messages_found ?>
        </div>
        
    <? else: ?>
        
        <div class="pm_items">
            <?
            $last_index = count($records) - 1;
            foreach($records as $index => $row):
                $author      = $accounts[$row->id_sender];
                $unread      = $row->opened_date == "0000-00-00 00:00:00" ? "unread" : "";
    
                if( $unread != "" ) $unread_ids[] = $row->id_pm;
                
                if( $index == 0 )             echo "<a name='first_pm' id='first_pm'></a>\n";
                elseif($index == $last_index) echo "<a name='last_pm'  id='last_pm'></a>\n";
                ?>
                <div class="pm_item <?= $unread ?> clearfix">
                    
                    <div class="avatar_col" style="float: left; width: 55px;">
                        <img class="user_avatar" width="50" src="<?= $author->get_avatar_url() ?>">
                    </div>
                    
                    <div class="pm_contents" style="float: left; margin-left: 5px; width: calc(100% - 60px)">
                        
                        <div class="top">
                            <span class="pull-right">
                                <?= get_selective_date($row->sent_date, true) ?>
                            </span>
                            <a class="meta_field user_display_name" data-user-level="<?= $author->level ?>"
                               href="<?= $config->full_root_path ?>/user/<?= $author->user_name ?>">
                                <span class="fa fa-user fa-fw"></span>
                                <?= $author->get_processed_display_name() ?>
                            </a>
                        </div>
                        
                        <div class="contents">
                            <?= $row->get_processed_contents() ?>
                        </div>
                    </div>
                    
                </div>
            <? endforeach; ?>
        </div>
        
    <? endif;  ?>
    
    <div class="pagination_button bottom">
        <?= $browser->get_pagination_button(
            "previous", "paginate_pms_browser", $pagination, "fa-caret-down"
        ) ?>
    </div>
    
</div>

<a name="send_pm_inlined_form"></a>
<form name="send_pm_paged_form" id="send_pm_paged_form" method="post"
      data-avoid-ajax-browser-hooks="true"
      action="<?= $config->full_root_path ?>/contact/send_pm.php"
      style="margin-top: 20px; padding: 10px; border: 1px solid silver; background-color: whitesmoke;">
    
    <input type="hidden" name="target" value="<?= $other_account->user_name ?>">
    <input type="hidden" name="no_sender_notification" value="true">
    
    <textarea name="content" class="expandible_textarea pm_input" style="min-height: 50px;"
              placeholder="<?= $current_module->language->send_pm_form->content->placeholder ?>"></textarea>
    
    <div class="attachments"></div>
    
    <div class="controls clearfix" style="margin-top: 10px;">
        <button type="submit" class="pull-right">
            <?= $current_module->language->pms_nav->actions->send ?>
            <span class="fa fa-play"></span>
        </button>
        
        <button onclick="add_pm_inlined_photo('image', 'fa-camera'); return false;">    
            <span class="fa fa-camera"></span>
            <?= $current_module->language->pms_nav->actions->add_photo ?>
        </button>
    </div>
    
</form>

<?
$unread = -1;
if( ! empty($unread_ids) )
{
    $repository->mark_as_read($unread_ids);
    # echo "<pre>" . print_r($repository->get_last_query(), true) . "</pre>";
    $unread = $repository->get_unread_count($account->id_account);
}
?>

<script type="text/javascript">
    $(document).ready(function()
    {
        $('.expandible_textarea').expandingTextArea();
        $('#send_pm_paged_form').ajaxForm({
            target:       '#pm_paged_form_target',
            beforeSubmit: prepare_pm_sending,
            success:      process_pm_response
        });
        
        if( '<?= (int) $offset ?>' == '0' || parseInt('<?= $_GET["previous_offset"] ?>') > parseInt('<?= $offset ?>') )
            $.scrollTo('#last_pm', 250);
        
        $('img.pm_attachment').each(function()
        {
            var src  = $(this).attr('src');
            var html = sprintf("<a href='%1$s' class='prettyPhoto'><img class='pm_attachment' src='%1$s'></a>", src);
            $(this).replaceWith(html);
        });
        
        var unread = parseInt('<?= $unread ?>');
        if( unread >= 0 ) update_pms_counter( parseInt('<?= $unread ?>') );
    });
</script>
