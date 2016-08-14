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
include "../lib/recaptcha-php-1.11/recaptchalib.php";

$accounts_repository = new accounts_repository();

$title = empty($_GET["target"])
       ? $current_module->language->title->general
       : $current_module->language->title->targeted;
$title = replace_escaped_vars($title, '{$site_name}', $settings->get("engine.website_name"));

$template->set_page_title($title);
$template->set("title", $title);
$template->set("page_tag", "contact_form");
$template->page_contents_include = "form.inc";
include "{$template->abspath}/main.php";
