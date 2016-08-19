<?php
/**
 * Module installation file
 *
 * @package    BardCanvas
 * @subpackage contact
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * Trailing vars:
 * @var string $module_install_action install, uninstall, enable, disable
 * @var array  $messages
 * @var array  $errors
 * @var module $this_module
 * @var module $current_module
 */

use hng2_base\module;

$tables = array(
    "pms" => "
        CREATE TABLE IF NOT EXISTS `pms` (
          `id_pm`              VARCHAR(32) NOT NULL DEFAULT '',
          `id_owner`           VARCHAR(32) NOT NULL DEFAULT '',
          `id_sender`          VARCHAR(32) NOT NULL DEFAULT '',
          `id_recipient`       VARCHAR(32) NOT NULL DEFAULT '',
          `sent_date`          DATETIME NOT NULL,
          `opened_date`        DATETIME NOT NULL,
          `sender_archived`    tinyint unsigned not null default 0,
          `recipient_archived` tinyint unsigned not null default 0,
          `contents`           TEXT,
          
          PRIMARY KEY            (`id_pm`),
          INDEX   `by_sender`    (`id_owner`, `id_sender`),
          INDEX   `by_recipient` (`id_owner`, `id_recipient`)
        ) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE='utf8mb4_unicode_ci'
    ",
    "pm_conversations" => "
        CREATE TABLE IF NOT EXISTS `pm_conversations` (
          `id_conversation`    VARCHAR(32) NOT NULL DEFAULT '',
          `id_owner`           VARCHAR(32) NOT NULL DEFAULT '',
          `id_other`           VARCHAR(32) NOT NULL DEFAULT '',
          `last_event_date`    DATETIME NOT NULL,
          `archived`           tinyint unsigned not null default 0,
        
          PRIMARY KEY                 (`id_conversation`),
          UNIQUE  INDEX   `by_thread` (`id_owner`, `id_other`)
        ) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE='utf8mb4_unicode_ci'
    ",
);

$init_settings = array();

if( $module_install_action == "install" )
{
    foreach($tables as $table_name => $query)
    {
        try
        {
            $database->exec($query);
            $messages[] = replace_escaped_vars($language->install_messages->table_created_ok, '{$table_name}', $table_name);
        }
        catch( \Exception $e )
        {
            $errors[] = replace_escaped_vars(
                $language->install_messages->table_created_ko,
                array( '{$table_name}', '{$error}' ),
                array(   $table_name,     $e->getMessage()  )
            );
            
            return;
        }
    }
    
    foreach($init_settings as $key => $val)
        if( $settings->get($key) != "" )
            $settings->set($key, $val);
    
    return;
}

if( $module_install_action == "uninstall" )
{
    foreach($tables as $table_name => $query)
    {
        try
        {
            $database->exec("DROP TABLE if exists {$table_name}");
            $messages[] = replace_escaped_vars($language->install_messages->table_deleted_ok, '{$table_name}', $table_name);
        }
        catch( \Exception $e )
        {
            $errors[] = replace_escaped_vars(
                $language->install_messages->table_deleted_ko,
                array( '{$table_name}', '{$error}' ),
                array(   $table_name,     $e->getMessage() )
            );
        }
    }
    
    foreach($init_settings as $key => $val) $settings->set($key, "");
    
    return;
}
