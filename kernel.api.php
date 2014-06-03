<?php

/**
 * @return array
 */
function hook_kernel_info() {
  $path = drupal_get_path('module', 'MODULE');
  $info = array(
    'app' => array(
      'kernel class' => 'AppKernel',
      'arguments' => array('prod', false),
      'calls' => array(
        'loadClassCache' => array(),
      ),
      'require_once' => array(
        $path .'/app/bootstrap.php.cache',
        $path .'/app/AppKernel.php',
      ),
      'console list' => $path .'/app.drush.json',
    ),
  );
  return $info;
}

