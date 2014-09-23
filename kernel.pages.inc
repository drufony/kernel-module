<?php
/**
 * @file
 * Symfony Kernel module page callbacks
 */

/**
 * Page callback that handles a Symfony request.
 *
 * @param \Symfony\Component\HttpFoundation\Request $request
 *   Request object created in hook_init().
 *
 * @return int|string
 *   Drupal response based on Symfony response.
 */
function kernel_callback(\Symfony\Component\HttpFoundation\Request $request) {
  $c = pimple_get_container('app');

  $kernel = $c['kernel'];
  $response = $kernel->handle($request);
  kernel_response($response);

  if ($response->isForbidden()) {
    return MENU_ACCESS_DENIED;
  }
  if ($response->isNotFound()) {
    return MENU_NOT_FOUND;
  }
  if ($response->isRedirection()) {
    $path = $response->headers->get('location');

    // Strip $base_url if it exists.
    if (0 === strpos($path, $GLOBALS['base_url'])) {
      $path = substr($path, strlen($GLOBALS['base_url']));
    }

    // Strip $base_path if it exists.
    if (0 === strpos($path, base_path())) {
      $path = substr($path, strlen(base_path()));
    }

    drupal_goto($path, array(), $response->getStatusCode());
  }
  if ($request->attributes->get('_drupal', false) && 0 === stripos($response->headers->get('Content-Type'), 'text/html')) {
    return $response->getContent();
  }
  else {
    $GLOBALS['devel_shutdown'] = FALSE;
    kernel_deliver_response($response);
  }
}

/**
 * Page callback that maps bundles assets path to conf_path location.
 */
function kernel_assets_callback() {
  $args = func_get_args();
  array_unshift($args, DRUPAL_ROOT, conf_path(), 'files', 'var', 'web', 'bundles');

  // Create a dummy file object so we can use Drupal function to make headers.
  $file = new stdClass();
  $file->filename = implode('/', $args);
  $file->filemime = file_get_mimetype($file->filename);
  $file->filesize = filesize($file->filename);

  /* @see file_transfer()
   *   which can't be used because it requires a stream wrapper and scheme
   */
  if (ob_get_level()) {
    ob_end_clean();
  }

  foreach (file_get_content_headers($file) as $name => $value) {
    drupal_add_http_header($name, $value);
  }
  drupal_send_headers();
  // Transfer file in 1024 byte chunks to save memory usage.
  if ($fd = fopen($file->filename, 'rb')) {
    while (!feof($fd)) {
      print fread($fd, 1024);
    }
    fclose($fd);
  }
  else {
    drupal_not_found();
  }
  drupal_exit();
}
