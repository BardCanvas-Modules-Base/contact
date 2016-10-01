<?php
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

include "../config.php";
include "../includes/bootstrap.inc";
if( ! $account->_exists ) throw_fake_401();

$template->page_contents_include = "pms.inc";
$template->set_page_title($current_module->language->pms_page_title);
$template->set("no_right_sidebar", true);
include "{$template->abspath}/main.php";
