<?php

/**
 * @return array
 */
function hook_kernel_info() {
  $path = drupal_get_path('module', 'MODULE');
  $info = array(
    'app' => array(
      'kernel class' => 'AppKernel',
      'environment' => 'prod',
      'debug' => false,
      'calls' => array(
        'loadClassCache' => array(),
      ),
      'autoload' => $path .'/app/autoload.php',
      'bootstrap cache' => $path .'/app/bootstrap.php.cache',
      'console list' => $path .'/app.drush.json',
    ),
  );
  return $info;
}

