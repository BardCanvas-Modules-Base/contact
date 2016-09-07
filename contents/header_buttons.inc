<?php
/**
 * Contact form header button
 *
 * @package    HNG2
 * @subpackage accounts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *             
 * @var settings $settings
 * @var config   $config
 * @var template $template
 */

use hng2_base\config;
use hng2_base\settings;
use hng2_base\template;
use hng2_modules\contact\pms_repository;

if( $settings->get("modules:contact.hide_on_main_menu") == "true" ) return;

$added_class = preg_match('#/contact/index.php#i', $_SERVER["PHP_SELF"]) ? "current" : "";
$template->add_menu_item(
    trim($this_module->language->menu_caption),
    "
    <a class='main_menu_item {$added_class}' data-module='contact' href='{$config->full_root_path}/contact/'>
        <span class='fa {$this_module->language->icon}'></span>
        {$this_module->language->menu_caption}
    </a>
    "
);

if( ! $account->_exists ) return;

$pms_repository  = new pms_repository();
$unread_pms      = $pms_repository->get_unread_count($account->id_account);
$added_class     = $unread_pms >  0 ? "alerted" : "";
$added_class    .= preg_match('#/contact/pms.php#i', $_SERVER["PHP_SELF"]) ? " current" : "";
$template->add_menu_item(
    "_____login_____",
    "
        <a id='unread_pms_count' class='main_menu_item always_visible $added_class pull-right' 
           data-module='contact' href='{$config->full_root_path}/contact/pms.php'
           title='{$this_module->language->read_pms}'>
           <span class='fa fa-inbox'></span>
           <span class='unread_count'>{$unread_pms}</span>
        </a>
        ",
    951
);
