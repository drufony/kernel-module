<?php

class DrupalKernelContainer
{
  private static $containers = array();

  public static function get($name = 'app')
  {
    if (!isset(self::$containers[$name])) {
      composer_manager_register_autoloader();
      $c = new \Pimple\Container();

      $hook = 'service_provider';
      $providers = array();

      foreach (module_implements($hook) as $module) {
        $function = $module . '_' . $hook;
        if (function_exists($function)) {
          $result = call_user_func_array($function, array($name));
          if (isset($result)) {
            $providers = array_merge($providers, (array) $result);
          }
        }
      }

      foreach ($providers as $args) {
        call_user_func_array(array($c, 'register'), (array) $args);
      }
      self::$containers[$name] = $c;
    }

    return self::$containers[$name];
  }
}
