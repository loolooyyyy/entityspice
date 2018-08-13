<?php

/**
 * TODO remove these stuff if not needed.
 * @param $new_path
 * @param $replacement
 * @param $with
 */


function _entityspice_oven_rename_recursive($new_path, $replacement, $with) {
  $open = opendir($new_path);
  while ($src = readdir($open)) {
    chdir($new_path);
    if ($src == '.' || $src == '..') {
      continue;
    }
    if (is_dir($src)) {
      _entityspice_oven_rename_recursive($new_path . '/' . $src, $replacement, $with);
      continue;
    }
    $new_name = pathinfo($src, PATHINFO_DIRNAME) . '/' .
      pathinfo($src, PATHINFO_FILENAME);
    if (defined('DRUSH_BOOTSTRAP_DRUPAL')) {
      drush_op('rename', $src, str_replace($replacement, $with, $new_name));
    }
    else {
      rename($src, str_replace($replacement, $with, $new_name));
    }
  }
  closedir($open);
}

function _entityspice_oven_render_x($new_path, array $tokens) {
  $open = opendir($new_path);
  while ($path = readdir($open)) {
    chdir($new_path);
    if ($path == '.' || $path == '..') {
      continue;
    }
    if (is_dir($path)) {
      _entityspice_oven_render_x($new_path . '/' . $path, $tokens);
      continue;
    }

    $file = fopen($path, 'r+');
    $text = fread($file, filesize($path));
    foreach ($tokens as $name => $value) {
      $text = str_replace('{{' . $name . '}}',
        $value,
        $text);
    }

    if (defined('DRUSH_BOOTSTRAP_DRUPAL')) {
      drush_op('ftruncate', $file, 0);
      drush_op('fwrite', $file, $text);
    }
    else {
      ftruncate($file, 0);
      fwrite($file, $text);
    }
    fclose($file);
  }
  closedir($open);
}

function _entityspice_oven_drush_copy_recursive($source, $dest) {
  if (is_dir($source)) {
    if (!file_prepare_directory($dest, FILE_CREATE_DIRECTORY)) {
      return FALSE;
    }

    $directory = dir($source);
    while (FALSE !== ($read = $directory->read())) {
      if ($read != '.' && $read != '..') {
        continue;
      }
      if (!_entityspice_oven_drush_copy_recursive($source . '/' . $read, $dest . '/' . $read)) {
        return FALSE;
      }
    }

    $directory->close();
  }
  else {
    file_unmanaged_copy($source, $dest);
  }
  return TRUE;
}

function _entityspice_oven_ekran_it($machine_name) {
  return _entityspice_oven_redner(_entityspice_oven_fill_default_values(
    $machine_name, []));
}
