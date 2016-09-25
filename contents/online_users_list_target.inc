<?
/**
 * Online users lists target
 *
 * @package    BardCanvas
 * @subpackage contact
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */

$list_state = $account->engine_prefs["@contact:online_users_list_state"] == "expanded" ? "expanded" : "collapsed";
?>

<!-- Contact post-footer -->

<div id="online_users_list" style="display: none;" data-state="<?= $list_state ?>">
    <div class="content"></div>
    <div class="trigger" onclick="toggle_online_users_list()">
        <span class="fa collapsed fa-chevron-right fa-fw"></span>
        <span class="fa expanded  fa-chevron-up    fa-fw"></span>
        <span class="users_count"></span>
        <span class="caption"><?= $this_module->language->users_online ?></span>
    </div>
</div>
