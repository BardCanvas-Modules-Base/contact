<?php
/**
 * Posts extender: after saving notifications
 *
 * @package    BardCanvas
 * @subpackage security
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * Imported globals:
 * @var posts_repository $repository
 * @var post_record      $post
 * @var post_record|null $old_post
 *
 * Trailing vars:
 * @var string $hook_area      save_post
 * @var string $hook_marker    after_saving
 * @var module $this_module    self (security)
 * @var module $current_module posts
 */

use hng2_base\config;
use hng2_base\module;
use hng2_modules\contact\toolbox;
use hng2_modules\posts\post_record;
use hng2_modules\posts\posts_repository;

global $template, $language, $settings, $mem_cache, $account, $post, $old_post, $repository;

# Notification to mods/admins of saving

$mem_key = "contact.notification_sent:post={$post->id_post}";
$mem_ttl = 60*60;

if( $account->level >= config::MODERATOR_USER_LEVEL ) return;
if( $post->status == "draft" ) return;
if( $mem_cache->get($mem_key) == "true" ) return;
if( $post->creation_date > date("Y-m-d H:i:s", strtotime("$post->creation_date + $mem_ttl seconds")) ) return;

$toolbox = new toolbox();

$config->globals["contact:posts/post"]        = $post;
$config->globals["contact:posts/post_author"] = $post->get_author();
$toolbox->notify_mods_on_post_submission();
unset( $config->globals["contact:posts/post_author"], $config->globals["contact:posts/post"] );
$mem_cache->set($mem_key, "true", 0, $mem_ttl);
