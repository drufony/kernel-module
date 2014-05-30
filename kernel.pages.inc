<?php

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
