<?php
/**
 * Restore all archived conversations for the current user
 *
 * @package    HNG2
 * @subpackage contact
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */

use hng2_base\accounts_repository;
use hng2_modules\contact\pms_repository;

include "../../config.php";
include "../../includes/bootstrap.inc";

if( ! $account->_exists ) throw_fake_401();

$pms_repository = new pms_repository();
$pms_repository->restore_archived_conversations($account->id_account);

echo "OK";
