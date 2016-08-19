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

$template->page_contents_include = "contents/pms_browser.inc";
$template->set_page_title($current_module->language->pms_page_title);
include "{$template->abspath}/embeddable.php";
