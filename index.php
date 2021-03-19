<?php
/**
 * Contact form
 *
 * @package    HNG2
 * @subpackage contact
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */

use hng2_base\accounts_repository;

include "../config.php";
include "../includes/bootstrap.inc";
session_start();

if( ! $modules["contact"]->enabled ) throw_fake_404();

$current_module->load_extensions("index", "pre_start");

$accounts_repository = new accounts_repository();

$title = empty($_GET["target"])
       ? $current_module->language->title->general
       : $current_module->language->title->targeted;
$title = replace_escaped_vars($title, '{$site_name}', $settings->get("engine.website_name"));

if( ! $account->_exists && $settings->get("modules:contact.csrf_for_guests") == "true" )
    $_SESSION["{$config->website_key}_contact_form_token"] = uniqid();

$template->set_page_title($title);
$template->set("title", $title);
$template->set("page_tag", "contact_form");
$template->set("no_right_sidebar", true);
$template->page_meta_robots = "noindex, nofollow";
$template->page_contents_include = "form.inc";
include "{$template->abspath}/main.php";
