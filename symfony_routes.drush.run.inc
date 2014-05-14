<?php
/**
 * @file
 * Provides the Composer integration in a seperate file so that PHP < 5.3 works.
 */

/**
 * Executes the given command in the current working directory.
 */
function drush_symfony_routes_app_run($arguments = array()) {
  // Append all the command line options (--option and -o).
  foreach ($_SERVER['argv'] as $index => $arg) {
    if ($arg[0] == '-') {
      $arguments[] = $arg;
    }
  }

  // Remove any invalid Composer options to construct the final arguments.
  $options = drush_symfony_routes_app_get_options(isset($arguments[0]) ? $arguments[0] : 'list');
  $args = array('app');
  foreach ($arguments as $index => $arg) {
    // Only act on options.
    if ($arg[0] == '-') {
      // Check if it's a valid option for the given command.
      foreach ($options as $name => $option) {
        // Validate it as a shortcut or option.
        if (stripos($arg, $option['name']) === 0 || stripos($arg, $option['shortcut']) === 0) {
          // Append it to the valid argument list.
          $args[] = $arg;
          // Continue on with the next argument.
          continue 2;
        }
      }
      // It's invalid, so do not add it into the final $args.
    }
    else {
      // Append all normal arguments to the valid list.
      $args[] = $arg;
    }
  }

  // Backup and override the server command line arguments.
  $backup = $_SERVER['argv'];
  $_SERVER['argv'] = $args;
  $_SERVER['argc'] = count($args);

  set_time_limit(0);

  $loader = require_once __DIR__.'/app/bootstrap.php.cache';
  require_once __DIR__.'/app/AppKernel.php';

  $input = new \Symfony\Component\Console\Input\ArgvInput();
  $env = $input->getParameterOption(array('--env', '-e'), getenv('SYMFONY_ENV') ?: 'dev');
  $debug = getenv('SYMFONY_DEBUG') !== '0' && !$input->hasParameterOption(array('--no-debug', '')) && $env !== 'prod';

  if ($debug) {
    \Symfony\Component\Debug\Debug::enable();
  }

  $kernel = new AppKernel($env, $debug);
  $application = new \Symfony\Bundle\FrameworkBundle\Console\Application($kernel);
  $application->run($input);

  // Now that Composer has run, restore the original server arguments.
  $_SERVER['argv'] = $backup;
  $_SERVER['argc'] = count($backup);

  return $loader;
}

/**
 * Retrieves any valid options available from Composer.
 *
 * @param string $command
 *   The command to get the options for.
 *
 * @return array
 *   A list of valid options for the given command.
 */
function drush_symfony_routes_app_get_options($command = 'list') {
  static $commands = array();

  // Retrieve all the commands available from Composer.
  if (empty($commands)) {
    $contents = file_get_contents(__DIR__ . '/app.drush.json');
    $json = json_decode($contents, TRUE);
    $commands = $json['commands'];
  }

  // Find the available options for the given command and the "help" command.
  $options = array();
  foreach ($commands as $index => $cmd) {
    // The name of the command determines what options are around.
    if ($cmd['name'] == $command || $cmd['name'] == 'help') {
      // Retrieve the options array and merge it into the given options.
      $array = isset($cmd['definition']['options']) ? $cmd['definition']['options'] : array();
      $options = array_merge($options, $array);
    }
  }

  return $options;
}
