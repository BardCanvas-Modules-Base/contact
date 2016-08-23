<?php
namespace hng2_modules\contact;

use hng2_base\account_record;
use hng2_media\media_repository;
use hng2_modules\comments\comment_record;
use hng2_modules\posts\posts_repository;

class toolbox
{
    public function notify_post_author_on_comment_submission()
    {
        global $config, $modules, $settings, $post, $comment;
        
        /**
         * @var account_record $post_author
         * @var account_record $comment_author
         */
        $post_author    = $config->globals["contact:comments/post_author"];
        $comment_author = $config->globals["contact:comments/comment_author"];
        
        $subject = replace_escaped_vars(
            $modules["contact"]->language->email_templates->comment_added->for_author->subject,
            array(
                '{$title}',
                '{$website_name}',
            ),
            array(
                $post->title,
                $settings->get("engine.website_name"),
            )
        );
        
        $body = replace_escaped_vars(
            $modules["contact"]->language->email_templates->comment_added->for_author->body,
            array(
                '{$author}',
                '{$comment_sender}',
                '{$post_title}',
                '{$comment}',
                '{$reply_url}',
                '{$report_url}',
                '{$preferences}',
                '{$website_name}',
                '{$post_link}',
            ),
            array(
                $post_author->display_name,
                empty($comment->id_author)
                    ? $comment->author_display_name
                    : "<a href='{$config->full_root_url}/user/{$comment_author->user_name}'>$comment_author->display_name</a>",
                $post->title,
                $comment->content,
                "{$config->full_root_url}/{$post->id_post}#comment_{$comment->id_comment}",
                "{$config->full_root_url}/contact/?action=report&type=comment&id={$comment->id_comment}",
                "{$config->full_root_url}/accounts/preferences.php",
                $settings->get("engine.website_name"),
                "{$config->full_root_url}/{$post->id_post}",
            )
        );
        
        $recipients = array($post_author->display_name => $post_author->email);
        send_mail($subject, $body, $recipients);
    }
    
    public function notify_mods_on_comment_submission()
    {
        global $config, $modules, $settings, $post, $comment;
        
        /**
         * @var account_record $post_author
         * @var account_record $comment_author
         */
        $post_author    = $config->globals["contact:comments/post_author"];
        $comment_author = $config->globals["contact:comments/comment_author"];
        
        $subject = replace_escaped_vars(
            $modules["contact"]->language->email_templates->comment_added->for_mods->subject,
            array(
                '{$website_name}',
                '{$author}',
                '{$title}',
            ),
            array(
                $settings->get("engine.website_name"),
                $post_author->display_name,
                $post->title,
            )
        );
        
        $body = replace_escaped_vars(
            $modules["contact"]->language->email_templates->comment_added->for_mods->body,
            array(
                '{$comment_sender}',
                '{$author}',
                '{$post_title}',
                '{$comment}',
                '{$reply_url}',
                '{$flag_url}',
                '{$preferences}',
                '{$website_name}',
                '{$post_link}',
            ),
            array(
                empty($comment->id_author)
                    ? $comment->author_display_name
                    : "<a href='{$config->full_root_url}/user/{$comment_author->user_name}'>$comment_author->display_name</a>",
                $post_author->display_name,
                $post->title,
                $comment->content,
                "{$config->full_root_url}/{$post->id_post}#comment_{$comment->id_comment}",
                "{$config->full_root_url}/comments/scripts/toolbox.php?action=change_status&new_status=spam&id_comment={$comment->id_comment}",
                "{$config->full_root_url}/accounts/preferences.php",
                $settings->get("engine.website_name"),
                "{$config->full_root_url}/{$post->id_post}",
            )
        );
        
        broadcast_mail_to_moderators($subject, $body, "@contact:moderator_emails_for_comments");
    }
    
    public function notify_parent_author_on_comment_reply()
    {
        global $config, $modules, $settings, $post, $comment;
        
        /**
         * @var comment_record $parent_comment
         * @var account_record $parent_author
         * @var account_record $post_author
         * @var account_record $comment_author
         */
        $parent_comment = $config->globals["contact:comments/parent_comment"];
        $parent_author  = $config->globals["contact:comments/parent_author"] ;
        $post_author    = $config->globals["contact:comments/post_author"]   ;
        $comment_author = $config->globals["contact:comments/comment_author"];
        
        $subject = replace_escaped_vars(
            $modules["contact"]->language->email_templates->comment_replied->for_parent_author->subject,
            array(
                '{$post_author}',
                '{$post_title}',
            ),
            array(
                $post_author->display_name,
                $post->title,
            )
        );
        
        $body = replace_escaped_vars(
            $modules["contact"]->language->email_templates->comment_replied->for_parent_author->body,
            array(
                '{$parent_author}',
                '{$comment_sender}',
                '{$post_author}',
                '{$post_link}',
                '{$post_title}',
                '{$parent_excerpt}',
                '{$comment}',
                '{$reply_url}',
                '{$report_url}',
                '{$preferences}',
                '{$website_name}',
            ),
            array(
                $parent_author->display_name,
                empty($comment->id_author)
                    ? $comment_author->author_display_name
                    : "<a href='{$config->full_root_url}/user/{$comment_author->user_name}'>$comment_author->display_name</a>",
                $post_author->display_name,
                "{$config->full_root_url}/{$post->id_post}",
                $post->title,
                make_excerpt_of($parent_comment->content, 255),
                $comment->content,
                "{$config->full_root_url}/{$post->id_post}#comment_{$comment->id_comment}",
                "{$config->full_root_url}/contact/?action=report&type=comment&id={$comment->id_comment}",
                "{$config->full_root_url}/accounts/preferences.php",
                $settings->get("engine.website_name"),
            )
        );
        
        $recipients = array($parent_author->display_name => $parent_author->email);
        send_mail($subject, $body, $recipients);
    }
    
    public function notify_mods_on_comment_reply()
    {
        global $config, $modules, $settings, $post, $comment;
        
        /**
         * @var comment_record $parent_comment
         * @var account_record $parent_author
         * @var account_record $post_author
         * @var account_record $comment_author
         */
        $parent_comment = $config->globals["contact:comments/parent_comment"];
        $parent_author  = $config->globals["contact:comments/parent_author"] ;
        $post_author    = $config->globals["contact:comments/post_author"]   ;
        $comment_author = $config->globals["contact:comments/comment_author"];
        
        $subject = replace_escaped_vars(
            $modules["contact"]->language->email_templates->comment_replied->for_mods->subject,
            array(
                '{$website_name}',
                '{$post_author}',
                '{$post_title}',
            ),
            array(
                $settings->get("engine.website_name"),
                $post_author->display_name,
                $post->title,
            )
        );
        
        $body = replace_escaped_vars(
            $modules["contact"]->language->email_templates->comment_replied->for_mods->body,
            array(
                '{$comment_sender}',
                '{$parent_author}',
                '{$post_author}',
                '{$post_link}',
                '{$post_title}',
                '{$parent_excerpt}',
                '{$comment}',
                '{$reply_url}',
                '{$flag_url}',
                '{$preferences}',
                '{$website_name}',
            ),
            array(
                empty($comment->id_author)
                    ? $comment_author->author_display_name
                    : "<a href='{$config->full_root_url}/user/{$comment_author->user_name}'>$comment_author->display_name</a>",
                $parent_author->display_name,
                $post_author->display_name,
                "{$config->full_root_url}/{$post->id_post}",
                $post->title,
                make_excerpt_of($parent_comment->content, 255),
                $comment->content,
                "{$config->full_root_url}/{$post->id_post}#comment_{$comment->id_comment}",
                "{$config->full_root_url}/comments/scripts/toolbox.php?action=change_status&new_status=spam&id_comment={$comment->id_comment}",
                "{$config->full_root_url}/accounts/preferences.php",
                $settings->get("engine.website_name"),
            )
        );
        
        broadcast_mail_to_moderators($subject, $body, "@contact:moderator_emails_for_comments");
    }
    
    public function notify_mods_on_post_submission()
    {
        /**
         * @var posts_repository $repository
         */
        global $config, $modules, $settings, $post, $repository;
        
        /**
         * @var account_record $post_author
         */
        $post_author = $config->globals["contact:posts/post_author"];
        
        $subject = replace_escaped_vars(
            $modules["contact"]->language->email_templates->post_submitted->subject,
            array(
                '{$website_name}',
                '{$post_author}',
                '{$post_title}',
            ),
            array(
                $settings->get("engine.website_name"),
                $post_author->display_name,
                $post->title,
            )
        );
        
        $user_ip  = get_user_ip(); $parts = explode(".", $user_ip); array_pop($parts);
        $segment  = implode(".", $parts);
        $boundary = date("Y-m-d H:i:s", strtotime("now - 7 days"));
        $where = array(
            "status = 'published'",
            "visibility = 'public'",
            "publishing_date >= '$boundary'",
            "creation_ip like '{$segment}.%'",
            "id_post <> '$post->id_post'",
        );
        $other_posts_from_segment = $repository->find($where, 12, 0, "publishing_date desc");
        if( count($other_posts_from_segment) == 0 )
        {
            $other_posts_from_segment = "<li>{$modules["contact"]->language->email_templates->post_submitted->none_found}</li>";
        }
        else
        {
            $lis = "";
            foreach($other_posts_from_segment as $other_post)
            {
                $published   = get_minimized_date($other_post->publishing_date);
                $link        = $other_post->get_permalink(true);
                $author_link = "{$config->full_root_url}/user/{$other_post->author_user_name}";
                $lis .= "<li><a href='$author_link'>{$other_post->author_display_name}</a>
                             [$published • {$other_post->creation_ip}]:
                             <a href='{$link}'>{$other_post->title}</a></li>";
            }
            $other_posts_from_segment = $lis;
        }
        
        $body = replace_escaped_vars(
            $modules["contact"]->language->email_templates->post_submitted->body,
            array(
                '{$post_author}',
                '{$main_category_url}',
                '{$main_category}',
                '{$post_link}',
                '{$post_title}',
                '{$excerpt}',
                '{$featured_image}',
                '{$ip}',
                '{$location}',
                '{$user_agent}',
                '{$other_posts_from_segment}',
                '{$post_url}',
                '{$edit_url}',
                '{$preferences}',
                '{$website_name}',
            ),
            array(
                "<a href='{$config->full_root_url}/user/{$post_author->user_name}'>$post_author->display_name</a>",
                "{$config->full_root_url}/category/{$post->main_category_slug}",
                $post->main_category_title,
                "{$config->full_root_url}/{$post->id_post}",
                $post->title,
                empty($post->excerpt) ? "&mdash;" : $post->excerpt,
                empty($post->featured_image_thumbnail)
                    ? "<p>{$modules["contact"]->language->email_templates->post_submitted->none_defined}</p>"
                    : "<img height='200' border='1' src='{$config->full_root_url}/mediaserver/{$post->featured_image_thumbnail}'>",
                $user_ip,
                forge_geoip_location($user_ip),
                $_SERVER["HTTP_USER_AGENT"],
                $other_posts_from_segment,
                "{$config->full_root_url}/{$post->id_post}",
                "{$config->full_root_url}/posts/?edit_post={$post->id_post}&wasuuup=" . md5(mt_rand(1, 65535)),
                "{$config->full_root_url}/accounts/preferences.php",
                $settings->get("engine.website_name"),
            )
        );
        
        broadcast_mail_to_moderators($subject, $body, "@contact:moderator_emails_for_posts");
    }
    
    public function notify_mods_on_media_submission()
    {
        /**
         * @var media_repository $repository
         */
        global $config, $modules, $settings, $item, $old_item, $repository;
        
        /**
         * @var account_record $item_author
         */
        $item_author = $config->globals["contact:media/item_author"];
        $type        = $modules["gallery"]->language->xpath("//media_types/media[@type='{$item->type}']/caption/text()");
        
        $subject = replace_escaped_vars(
            $modules["contact"]->language->email_templates->media_item_submitted->subject,
            array(
                '{$website_name}',
                '{$type}',
                '{$item_author}',
                '{$title}',
            ),
            array(
                $settings->get("engine.website_name"),
                $type,
                $item_author->display_name,
                $item->title,
            )
        );
        
        $user_ip  = get_user_ip(); $parts = explode(".", $user_ip); array_pop($parts);
        $segment  = implode(".", $parts);
        $boundary = date("Y-m-d H:i:s", strtotime("now - 7 days"));
        $where = array(
            "status = 'published'",
            "visibility = 'public'",
            "publishing_date >= '$boundary'",
            "creation_ip like '{$segment}.%'",
            "id_media <> '$item->id_media'",
        );
        $other_from_segment = $repository->find($where, 12, 0, "publishing_date desc");
        if( count($other_from_segment) == 0 )
        {
            $other_from_segment = "<p>{$modules["contact"]->language->email_templates->media_item_submitted->none_found}</p>";
        }
        else
        {
            $lis = "";
            foreach($other_from_segment as $other_item)
            {
                $published = get_minimized_date($other_item->publishing_date);
                $link      = $other_item->get_page_url(true);
                $lis      .= "
                    <span style='display: inline-block; width: 150px; vertical-align: top; margin: 10px;'>
                        <a href='{$link}'><img width='150' border='1' src='{$other_item->get_thumbnail_url(true)}'></a><br>
                        <a href='{$config->full_root_url}/user/{$other_item->author_user_name}'>{$other_item->author_display_name}</a><br>
                        [$published • {$other_item->creation_ip}]:<br>
                        <a href='{$link}'>{$other_item->title}</a>
                    </span>
                ";
            }
            $other_from_segment = $lis;
        }
        
        $body = replace_escaped_vars(
            $modules["contact"]->language->email_templates->media_item_submitted->body,
            array(
                '{$item_author}',
                '{$type}',
                '{$thumbnail}',
                '{$title}',
                '{$description}',
                '{$ip}',
                '{$location}',
                '{$user_agent}',
                '{$other_from_segment}',
                '{$item_url}',
                '{$edit_url}',
                '{$preferences}',
                '{$website_name}',
            ),
            array(
                "<a href='{$config->full_root_url}/user/{$item_author->user_name}'>$item_author->display_name</a>",
                $type,
                $item->get_thumbnail_url(true),
                $item->title,
                $item->description,
                $user_ip,
                forge_geoip_location($user_ip),
                $_SERVER["HTTP_USER_AGENT"],
                $other_from_segment,
                $item->get_page_url(true),
                "{$config->full_root_url}/gallery/?edit_item={$item->id_media}&wasuuup=" . md5(mt_rand(1, 65535)),
                "{$config->full_root_url}/accounts/preferences.php",
                $settings->get("engine.website_name"),
            )
        );
        
        broadcast_mail_to_moderators($subject, $body, "@contact:moderator_emails_for_posts");
    }
}
