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

<h1 class="clearfix">
    <button class="pull-right" onclick="location.href = '<?= $_SERVER["PHP_SELF"] ?>?wasuuup=<?= md5(mt_rand(1, 65535)) ?>'">
        <span class="fa fa-refresh"></span>
        <?= $language->reload ?>
    </button>
    
    <?= $current_module->language->pms_page_title ?>
</h1>

<? if( count($conversations) == 0 ): ?>
    <div class="framed_content state_highlight">
        <span class="fa fa-info-circle"></span>
        <?= $current_module->language->messages->no_conversations ?>
    </div>
    <? return; ?>
<? endif; ?>

<div id="pms_viewer" class="clearfix">
    <div class="conversations">
        <? foreach($conversations as $conversation):
            $author   = $authors[$conversation->id_other];
            $selected = $author->user_name == $_GET["with"] ? "selected" : "";
            ?>
            
            <div class="meta_box conversation_item <?= $selected ?> clearfix">
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
        <p>
        Texto del último mensaje o la conversación elegida
        </p>
    </div>
</div>
