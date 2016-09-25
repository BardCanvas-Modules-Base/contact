<?php
/**
 * Contacts - extender to inject PM status modifiers to the notifications getter
 * Note: this file is injected directly, not called within any function
 *
 * @package    BardCanvas
 * @subpackage contact
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var config $config
 * @var module $current_module                  Underlaying module
 * @var module $this_module                     Self
 * @var array  $notifications_return_collection Notifications return array
 */

use hng2_base\accounts_repository;
use hng2_base\config;
use hng2_base\module;
use hng2_modules\contact\pms_repository;

if( ! $account->_exists ) return;

$pms_repository = new pms_repository();
$unread_count   = $pms_repository->get_unread_count($account->id_account);
if( $unread_count > 0 ) $notifications_return_collection["unread_pms"] = $unread_count;

$accounts_repository = new accounts_repository();
$users               = $accounts_repository->get_online_users_list();
if( count($users) > 0 ) $notifications_return_collection["online_users"] = $users;
