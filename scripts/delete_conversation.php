<?php
/**
 * Deletes the conversation bewteen the online user and another user
 *
 * @package    HNG2
 * @subpackage contact
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *             
 * $_GET params:
 * @param string "with"         other user name
 * @param bool   "archive_only" to archive instead of delete
 */

use hng2_base\accounts_repository;
use hng2_modules\contact\pms_repository;

include "../../config.php";
include "../../includes/bootstrap.inc";

if( ! $account->_exists ) throw_fake_401();

if( empty($_GET["with"]) ) die($current_module->language->messages->missing_target);

$pms_repository      = new pms_repository();
$accounts_repository = new accounts_repository();
$other = $accounts_repository->get($_GET["with"]);

if($_GET["archive_only"] == "true")
    $pms_repository->archive_conversation_by_partners($account->id_account, $other->id_account);
else
    $pms_repository->delete_conversation_by_parnters($account->id_account, $other->id_account);

echo "OK";
