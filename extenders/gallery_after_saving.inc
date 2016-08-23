<?php
/**
 * Gallery extender: after saving notifications (when adding/saving media items)
 * Data comes in $_POST
 *
 * @package    BardCanvas
 * @subpackage security
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * Imported globals:
 * @var media_record      $item
 * @var media_record|null $old_item
 * @var media_repository  $repository
 *
 * Trailing vars:
 * @var string $hook_area      save_item
 * @var string $hook_marker    after_saving
 * @var module $this_module    self (security)
 * @var module $current_module gallery
 */

use hng2_base\config;
use hng2_base\module;
use hng2_media\media_record;
use hng2_media\media_repository;
use hng2_modules\contact\toolbox;

global $template, $language, $settings, $mem_cache, $account, $item, $old_item, $repository;

# Notification to mods/admins of saving directly on the media browser (embedded or not)

$mem_key = "contact.notification_sent:media_item={$item->id_media}";
if( $comment_author->level >= config::MODERATOR_USER_LEVEL ) return;
if( $mem_cache->get($mem_key) == "true" ) return;

$toolbox = new toolbox();
$mem_ttl = 60*60;

$config->globals["contact:media/item_author"] = $item->get_author();
$toolbox->notify_mods_on_media_submission();
unset( $config->globals["contact:media/item_author"] );
$mem_cache->set($mem_key, "true", 0, $mem_ttl);
