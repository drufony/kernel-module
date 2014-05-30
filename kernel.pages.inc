<?php

/**
 * @param \Symfony\Component\HttpFoundation\Request $request
 * @return int|string
 */
function kernel_callback(\Symfony\Component\HttpFoundation\Request $request) {
  $kernel = kernel_app();
  $response = $kernel->handle($request);
  kernel_response($response);

  if ($response->isForbidden()) {
    return MENU_ACCESS_DENIED;
  }
  if ($response->isNotFound()) {
    return MENU_NOT_FOUND;
  }
  if ($response->isRedirection()) {
    $path = substr($response->headers->get('location'), strlen($GLOBALS['base_url'] . base_path()));
    drupal_goto($path);
  }
  if (0 === stripos($response->headers->get('Content-Type'), 'text/html')) {
    return $response->getContent();
  }
  else {
    $GLOBALS['devel_shutdown'] = FALSE;
    kernel_deliver_response($response);
  }
}

function kernel_assets_callback() {
  // @todo Don't hard code assumption about kernel name.
  $name = 'app';
  $args = func_get_args();
  array_unshift($args, DRUPAL_ROOT, conf_path(), $name, 'web', 'bundles');

  // Create a dummy file object so we can use Drupal function to make headers.
  $file = new stdClass;
  $file->filename = implode('/', $args);
  $file->filemime = file_get_mimetype($file->filename);
  $file->filesize = filesize($file->filename);

  /* @see file_transfer() which can't be used because it requires a stream wrapper and scheme */
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
