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
if( $comment_author->level >= config::MODERATOR_USER_LEVEL ) return;
if( $mem_cache->get($mem_key) == "true" ) return;

$toolbox = new toolbox();
$mem_ttl = 60*60;

$config->globals["contact:posts/post_author"] = $post->get_author();
$toolbox->notify_mods_on_post_submission();
unset( $config->globals["contact:posts/post_author"] );
$mem_cache->set($mem_key, "true", 0, $mem_ttl);
