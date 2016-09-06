<?php
/**
 * Comments extender: after saving notifications for comments flagged "for review"
 *
 * @package    BardCanvas
 * @subpackage contact
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * Imported globals:
 * @var comments_repository $repository
 * @var comment_record      $comment
 * @var comment_record      $old_comment Only when $hook_area == save_comment
 * @var post_record         $post
 *
 * Trailing vars:
 * @var string $hook_area      add_comment|save_comment
 * @var string $hook_marker    after_saving
 * @var module $this_module    self (security)
 * @var module $current_module comments
 */

use hng2_base\config;
use hng2_base\module;
use hng2_modules\comments\comment_record;
use hng2_modules\comments\comments_repository;
use hng2_modules\contact\toolbox;
use hng2_modules\posts\post_record;

global $template, $language, $settings, $account, $comment, $old_comment, $post, $repository, $mem_cache;

$toolbox = new toolbox();
$mem_ttl = 60*60;

# Notification to mods on post comments
if( empty($comment->parent_comment) )
{
    $post_author    = $post->get_author();
    $comment_author = $comment->get_author();
    $mem_key        = "contact.notification_sent:comment={$comment->id_comment}";
    
    if( $mem_cache->get($mem_key) != "true" )
    {
        $config->globals["contact:comments/post_author"]    = $post_author;
        $config->globals["contact:comments/comment_author"] = $comment_author;
        
        # To mods/admins
        if( $comment_author->level < config::MODERATOR_USER_LEVEL )
            $toolbox->notify_mods_on_comment_for_review();
        
        # Finishing touches
        unset(
            $config->globals["contact:comments/post_author"],
            $config->globals["contact:comments/comment_author"]
        );
        $mem_cache->set($mem_key, "true", 0, $mem_ttl);
    }
    
    return;
}

# Notification to mods on comment replies
if( ! empty($comment->parent_comment) )
{
    $parent_comment = $repository->get($comment->parent_comment);
    $parent_author  = $parent_comment->get_author();
    $post_author    = $post->get_author();
    $comment_author = $comment->get_author();
    $mem_key        = "contact.notification_sent:parent_comment={$parent_comment->id_comment},child={$comment->id_comment}";
    
    if( $mem_cache->get($mem_key) != "true" )
    {
        $config->globals["contact:comments/parent_comment"] = $parent_comment;
        $config->globals["contact:comments/parent_author"]  = $parent_author;
        $config->globals["contact:comments/post_author"]    = $post_author;
        $config->globals["contact:comments/comment_author"] = $comment_author;
        
        # To mods/admins
        if( $comment_author->level < config::MODERATOR_USER_LEVEL )
            $toolbox->notify_mods_on_reply_for_review();
        
        # Finishing
        unset(
            $config->globals["contact:comments/parent_comment"] ,
            $config->globals["contact:comments/parent_author"]  ,
            $config->globals["contact:comments/post_author"]    ,
            $config->globals["contact:comments/comment_author"]
        );
        
        $mem_cache->set($mem_key, "true", 0, $mem_ttl);
    }
    
    return;
}
