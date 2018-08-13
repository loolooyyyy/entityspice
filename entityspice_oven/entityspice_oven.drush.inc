<?php

require_once 'entityspice_oven.module';

/**
 * Asks user for yes or no and parse input.
 *
 * Accepts all forms of yes and no, y, Y, 1, t, true, TRUE, false, FALSE, ...
 *
 * @param bool $check_context
 * @param string $msg message to display as prompt.
 *
 * @return bool true if user said yes false otherwise.
 */
function _entityspice_oven_drush_presume($check_context, $msg) {
  drush_print_prompt($msg . " (Y/n): ", 0);
  if ($check_context && drush_get_context('DRUSH_AFFIRMATIVE')) {
    // Automatically accept confirmations if the --yes argument was supplied.
    drush_print("y");
    return TRUE;
  }
  elseif ($check_context && drush_get_context('DRUSH_NEGATIVE')) {
    // Automatically cancel confirmations if the --no argument was supplied.
    drush_print("n");
    return FALSE;
  }
  else {
    $stdin = fopen("php://stdin", "r");
    while ($line = fgets($stdin)) {
      $line = trim($line);
      if (empty($line) || $line == 'y') {
        return TRUE;
      }
      if ($line == 'n') {
        return FALSE;
      }
      drush_print_prompt($msg . " (Y/n): ", 0);
    }
  }
  return FALSE;
}

// _____________________________________________________________________________

/**
 * Implements hook_drush_command().
 */
function entityspice_oven_drush_command() {
  /** @noinspection PhpUndefinedConstantInspection */
  $items['entityspice-oven-cook'] = [
    'aliases' => ['cook'],
    'callback' => '_entityspice_oven_drush_ekran',
    'description' => 'Create a module containing an entity (cook an entity type).',
    'arguments' => [_ENTITYSPICE_MACHINE => 'Machine name of generated entity.'],
    'options' => [],
    'examples' => [],
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  ];
  $items['kook'] = [
    'callback' => 'kook',
    'description' => 'debug',
    'arguments' => [],
    'options' => [],
    'examples' => [],
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  ];
  foreach (_entityspice_oven_tokens() as $name => $token) {
    if ($name !== _ENTITYSPICE_MACHINE) {
      $items['entityspice-oven-cook']['options'][$name] = $token['description'];
    }
  }
  return $items;
}

/**
 * Implements hook_drush_help().
 *
 * @param $section
 */
function entityspice_oven_drush_help($section) {
  // @TODO
}

/**
 * drush command callback.
 *
 * @param string $machine_name
 *
 * @return bool true on success, false otherwise.
 */
function _entityspice_oven_drush_ekran($machine_name = NULL) {
  // ======================================================= Handle machine name
  if ($err = _entityspice_oven_token_validate(_ENTITYSPICE_MACHINE, $machine_name)) {
    drush_set_error('bad machine name: ' . _entityspice_oven_error_msg($err));
    return FALSE;
  }
  if (module_exists($machine_name)) {
    drush_set_error("a module names [$machine_name] already exists");
    return FALSE;
  }
  $tokens[_ENTITYSPICE_MACHINE] = $machine_name;

  // ================================================== Handle drush cli options
  foreach (_entityspice_oven_tokens() as $name => $token) {
    // Machine name is already set by a mandatory argument.
    if ($name === _ENTITYSPICE_MACHINE) {
      continue;
    }
    $arg = drush_get_option($name, NULL);
    if ($arg === NULL) {
      continue;
    }
    $arg = _entityspice_oven_token_post_process($token['validation_policy'], $arg);
    if ($err = _entityspice_oven_token_validate($name, $arg)) {
      drush_set_error('bad value: ' . $arg . ', ' . _entityspice_oven_error_msg($err));
      return FALSE;
    }
    else {
      $tokens[$name] = $arg;
    }
  }

  // ======================================================= Handle drush prompt
  if (_entityspice_oven_drush_presume(TRUE, 'do you want to use default options?')) {
    $tokens = _entityspice_oven_fill_default_values($machine_name, $tokens);
  }
  else {
    // Uses 'prompt', 'label', 'description' in tokens array.
    drush_log('Enter "?" to see each arguments help, ' .
      'leave empty for default value.' . "\n", 'ok');
    foreach (_entityspice_oven_tokens() as $name => $token) {
      if (isset($tokens[$name]) || !$token['prompt']) {
        continue;
      }
      while (TRUE) {
        $value = drush_prompt($token['label'] . ' (? for help)',
          _entityspice_oven_token_default_value($machine_name, $name),
          TRUE);
        if ($value === '?') {
          drush_log($token['description'] . "\n", 'ok');
          continue;
        }
        $value = _entityspice_oven_token_post_process($token['validation_policy'], $value);
        if ($err = _entityspice_oven_token_validate($name, $value)) {
          drush_log(_entityspice_oven_error_msg($err), 'error');
        }
        else {
          $tokens[$name] = $value;
          break;
        }
      };
    }
  }

  // ======================================================= Find dest directory
  $dest = _entityspice_oven_pm_download_destination('module') . '/' . $machine_name;
  if (is_dir($dest)) {
    drush_set_error("destination already exists: $dest, installing into an existing" .
      ' destination is dangerous and not supported');
    return FALSE;
  }

  // ===================================================================== EKRAN
  $tmp = drush_tempdir() . '/' . $machine_name;
  if ($err = _entityspice_oven_ekran($tmp, $tokens)) {
    drush_log("failed: $err", 'error');
    return FALSE;
  }

  drush_move_dir($tmp, $dest);
  drush_log('FIN, your beautiful entity type is generated at: ' . $dest, 'ok');

  return TRUE;
}

// TODO for debug. Remove after dev is done.
function kook() {
}

