<?php
/**
 * Contact form header button
 *
 * @package    HNG2
 * @subpackage contact
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *             
 * @var settings $settings
 * @var config   $config
 * @var template $template
 */

use hng2_base\config;
use hng2_base\settings;
use hng2_base\template;

if( $settings->get("modules:contact.hide_on_main_menu") != "true" )
{
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
}
