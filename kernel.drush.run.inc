<?php
/**
 * @file
 * Provides the Composer integration in a seperate file so that PHP < 5.3 works.
 */

/**
 * Executes the given command in the current working directory.
 */
function drush_kernel_app_run($arguments = array()) {
  // Append all the command line options (--option and -o).
  foreach ($_SERVER['argv'] as $argument) {
    if ($argument[0] == '-') {
      $arguments[] = $argument;
    }
  }

  // Remove any invalid command options to construct the final arguments.
  $options = drush_kernel_app_get_options(isset($arguments[0]) ? $arguments[0] : 'list');
  $args = array('app');
  foreach ($arguments as $argument) {
    // Only act on options.
    if ($argument[0] == '-') {
      // Check if it's a valid option for the given command.
      foreach ($options as $option) {
        // Validate it as a shortcut or option.
        if (stripos($argument, $option['name']) === 0 || stripos($argument, $option['shortcut']) === 0) {
          // Append it to the valid argument list.
          $args[] = $argument;
          // Continue on with the next argument.
          continue 2;
        }
      }
      // It's invalid, so do not add it into the final $args.
    }
    else {
      // Append all normal arguments to the valid list.
      $args[] = $argument;
    }
  }

  // Backup and override the server command line arguments.
  $backup = $_SERVER['argv'];
  $_SERVER['argv'] = $args;
  $_SERVER['argc'] = count($args);

  set_time_limit(0);

  $input = new \Symfony\Component\Console\Input\ArgvInput();
  $env = $input->getParameterOption(array('--env', '-e'), getenv('SYMFONY_ENV') ?: 'dev');
  $debug = getenv('SYMFONY_DEBUG') !== '0' && !$input->hasParameterOption(array('--no-debug', '')) && $env !== 'prod';

  if ($debug) {
    \Symfony\Component\Debug\Debug::enable();
  }

  $kernel = kernel_app();
  if (is_a($kernel, 'Symfony\Component\HttpKernel\KernelInterface')) {
    /** @var $kernel Symfony\Component\HttpKernel\KernelInterface */
    $application = new \Symfony\Bundle\FrameworkBundle\Console\Application($kernel);
    $application->setAutoExit(false);
    $application->run($input);
  }

  // Now that Composer has run, restore the original server arguments.
  $_SERVER['argv'] = $backup;
  $_SERVER['argc'] = count($backup);
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
function drush_kernel_app_get_options($command = 'list') {
  static $commands = array();

  // Retrieve all the commands available from Composer.
  if (empty($commands)) {
    $info = kernel_info();
    if (!file_exists($info['console list'])) {
      $file = $info['console list'];
      $kernel = kernel_app();
      if (is_a($kernel, 'Symfony\\Component\\HttpKernel\\KernelInterface')) {
        /** @var $kernel Symfony\Component\HttpKernel\KernelInterface */
        $application = new \Symfony\Bundle\FrameworkBundle\Console\Application($kernel);
        $output = new \Symfony\Component\Console\Output\StreamOutput(fopen($file, 'a', false));
        $helper = new \Symfony\Component\Console\Helper\DescriptorHelper();
        $helper->describe($output, $application, array(
          'format' => 'json',
        ));
      }
    }

    $contents = file_get_contents($info['console list']);
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
