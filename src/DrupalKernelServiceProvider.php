<?php

use Pimple\ServiceProviderInterface;

class DrupalKernelServiceProvider implements ServiceProviderInterface
{
  /**
   * Registers services on the given container.
   *
   * This method should only be used to configure services and parameters.
   * It should not get services.
   *
   * @param \Pimple\Container $pimple An Container instance
   */
  public function register(\Pimple\Container $pimple)
  {
    $pimple['kernel.name'] = 'app';
    $pimple['kernel.info'] = function ($c) {
      return kernel_info($c['kernel.name']);
    };

    $pimple['kernel'] = function ($c) {
      $info = $c['kernel.info'];
      if (is_array($info['require_once'])) {
        $files = array_filter($info['require_once'], 'file_exists');
        if (!empty($files)) {
          foreach ($files as $file) {
            require_once $file;
          }
        }
      }

      if ($info['arguments'][1]) {
        \Symfony\Component\Debug\Debug::enable();
      }

      if (class_exists($info['kernel class'])) {

        /* @var \Symfony\Component\HttpKernel\KernelInterface $kernel */
        if (!empty($info['arguments']) && is_array($info['arguments'])) {
          $reflect = new ReflectionClass($info['kernel class']);
          $kernel = $reflect->newInstanceArgs($info['arguments']);
        } else {
          $kernel  = new $info['kernel class']();
        }

        foreach ($info['calls'] as $method => $args) {
          call_user_func_array(array($kernel, $method), $args);
        }

        return $kernel;
      } else {
        throw new \ErrorException($info['kernel class'] .' is not found');
      }

    };

    $pimple['container'] = function ($c) {
      $c['kernel']->boot();

      return $c['kernel']->getContainer();
    };

    $pimple['router'] = function ($c) {
      return $c['container']->get('router');
    };
    $pimple['router.transformer'] = $pimple->protect(
    /**
     * @param $path
     *
     * @return array
     */
      function ($path) {
        return array(
          'path' => $path,
          'page_callback' => 'kernel_callback',
          'page_arguments' => array(\Symfony\Component\HttpFoundation\Request::createFromGlobals()),
          'delivery_callback' => '',
          'tab_parent' => '',
          'tab_root' => $path,
          'title' => '',
          'type' => MENU_CALLBACK,
          'include_file' => drupal_get_path('module', 'kernel') . '/kernel.pages.inc',
          'href' => $path,
          'tab_root_href' => $path,
          'tab_parent_href' => '',
          'access' => TRUE,
          'original_map' => arg(NULL, $path),
          'map' => arg(NULL, $path),
        );
      });
  }
}
