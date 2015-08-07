<?php
/**
 * @file
 * Drush wrapper around the Composer package management system.
 *
 * @author Rob Loach (http://robloach.net)
 */

/**
 * Implementation of hook_drush_command().
 */
function kernel_drush_command() {
  $items['app'] = array(
    'description' => 'Perform package management and dependency tracking of your projects and libraries.',
    'arguments' => array(
      'command' => 'Run "drush app" for a list of available commands.',
    ),
    'allow-additional-options' => TRUE,
    'examples' => array(
      'drush app' => 'Display available commands available with Application.',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );

  return $items;
}

/**
 * Drush Composer; Invokes a Composer command.
 *
 * @param $command
 *   The Composer command we are to execute.
 */
function drush_kernel_app($command = '') {
  // Make sure the PHP requirements are met.
  $php = '5.3.2';
  $current = phpversion();
  if (version_compare($current, $php, '<')) {
    drush_set_error('DRUSH_APPLICATION_ERROR', dt('Composer requires at least PHP !php in order to run properly. You are currently on PHP !current.', array(
      '!php' => $php,
      '!current' => $current,
    )));

    return FALSE;
  }

  // Include the run in a different file so that PHP versions < 5.3 do not fail
  // parsing the file.
  require_once(__DIR__ .'/kernel.drush.run.inc');

  // Run the console command using the given function arguments.
  return drush_kernel_app_run(func_get_args());
}
