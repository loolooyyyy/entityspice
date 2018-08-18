<?php /** @noinspection PhpUnhandledExceptionInspection */

const _ENTITYSPICE_OVEN_FILES = [
  'class.inc',
  'bundle.class.inc',
  'entity.inc',
  'forms.inc',
  'info',
  'inline_entity_form.inc',
  'install',
  'module',
];

const _ENTITYSPICE_OVEN_MODULE_NAME = 'module_name';
const _ENTITYSPICE_OVEN_MODULE_DESC = 'module_description';
const _ENTITYSPICE_OVEN_MODULE_PACK = 'module_package';

const _ENTITYSPICE_OVEN_FORM_REDIRECT_URI = 'form_redirect_uri';
const _ENTITYSPICE_OVEN_ADMIN_BUNDLE_URI = 'admin_bundle_uri';
const _ENTITYSPICE_OVEN_DEFAULT_URI = 'default_uri';
const _ENTITYSPICE_OVEN_DEFAULT_MODULE_PACKAGE = 'default_module_package';
const _ENTITYSPICE_OVEN_DEFAULT_MODULE_DESCRIPTION = 'default_module_description';

const _ENTITYSPICE_MACHINE = 'machine_name';
const _ENTITYSPICE_OVEN_MACHINE_UPPER = 'machine_upper';
const _ENTITYSPICE_OVEN_MACHINE_CAMEL = 'machine_camel';
const _ENTITYSPICE_OVEN_LABEL = 'label';
const _ENTITYSPICE_OVEN_TYPE_CONST = 'type_const';

const _ENTITYSPICE_OVEN_CUSTOM_PROPERTIES = [
  'revision',
  'bundle',
  'uid',
  'created',
  'updated',
  'title',
];

//const _ENTITYSPICE_OVEN_MENU_LABEL = 'menu_label';

//const _ENTITYSPICE_OVEN_CONTENT_DEFAULT_URI = 'content_default_uri';
//const _ENTITYSPICE_OVEN_ADMIN_BUNDLE_URI = 'admin_bundle_uri';
//const _ENTITYSPICE_OVEN_DELETE_FORM_REDIRECT_URI = 'delete_form_redirect_uri';

require_once 'entityspice_oven_error.inc';

// _____________________________________________________________________________

function _entityspice_oven_token_default_value($machine_name, $name) {
  $tokens = _entityspice_oven_tokens();

  $camel = str_replace(
    '_', '', ucwords($machine_name, '_'));

  switch ($tokens[$name]['default_policy']) {
    case 'error':
      throw new Exception("don't know how to generate default value for: $name");
    case 'empty_string':
      return '';
    case 'boolean_true':
      return TRUE;
    case 'boolean_false':
      return FALSE;

    case _ENTITYSPICE_MACHINE:
      return $machine_name;
    case _ENTITYSPICE_OVEN_MACHINE_UPPER:
      return strtoupper($machine_name);
    case _ENTITYSPICE_OVEN_MACHINE_CAMEL:
      return $camel;
    case _ENTITYSPICE_OVEN_LABEL:
      return str_replace('_', ' ', ucwords($machine_name, '_'));
    case _ENTITYSPICE_OVEN_TYPE_CONST:
      return '_ET_' . strtoupper($machine_name);
    case _ENTITYSPICE_OVEN_MODULE_DESC:
      return "Module for entity type " . $camel;

    // TODO
    case _ENTITYSPICE_OVEN_FORM_REDIRECT_URI:
      return '/';
    case _ENTITYSPICE_OVEN_ADMIN_BUNDLE_URI:
      return "admin/$machine_name" . '_type';
    case _ENTITYSPICE_OVEN_DEFAULT_URI:
      return "entity/$machine_name";
    case _ENTITYSPICE_OVEN_DEFAULT_MODULE_PACKAGE:
      return 'Custom Entity Types';
    case _ENTITYSPICE_OVEN_DEFAULT_MODULE_DESCRIPTION:
      return 'This module defines the entity type ' . $camel;

    case _ENTITYSPICE_KEY_CREATED:
      return 'created';
    case _ENTITYSPICE_KEY_UPDATED:
      return 'updated';
    case _ENTITYSPICE_KEY_REVISION:
      return 'rid';
    case _ENTITYSPICE_KEY_TITLE:
      return 'title';
    case _ENTITYSPICE_KEY_UID:
      return 'uid';
    case _ENTITYSPICE_KEY_BUNDLE:
      return 'bundle';

    default:
      $policy = $tokens[$name]['default_policy'];
      if (function_exists($policy)) {
        return $policy($machine_name);
      }
      else {
        throw new Exception("don't know how to generate default value for: $name");
      }
  }
}

function _entityspice_oven_tokens() {
  $tokens[_ENTITYSPICE_MACHINE] = [
    'prompt' => FALSE,
    'label' => 'machine name',
    'description' => "machine readable name of module (and it's entity), " .
      "must be only letters (small caps), numbers and underscores. It " .
      "shouldn't start with a number",
    'validation_policy' => _ENTITYSPICE_MACHINE,
    'default_policy' => _ENTITYSPICE_MACHINE,
  ];
  $tokens[_ENTITYSPICE_OVEN_MACHINE_CAMEL] = [
    'prompt' => FALSE,
    'label' => 'MachineReadableCameCaseName',
    'description' => 'must exactly match the regular machine name but ' .
      'in camel case',
    'validation_policy' => _ENTITYSPICE_OVEN_MACHINE_CAMEL,
    'default_policy' => _ENTITYSPICE_OVEN_MACHINE_CAMEL,
  ];
  $tokens[_ENTITYSPICE_OVEN_MACHINE_UPPER] = [
    'prompt' => FALSE,
    'label' => 'MACHINE_NAME_UPPER_CASE',
    'description' => 'must exactly match the regular machine name but ' .
      'in upper case',
    'validation_policy' => 'name',
    'default_policy' => _ENTITYSPICE_OVEN_MACHINE_UPPER,
  ];
  $tokens[_ENTITYSPICE_OVEN_TYPE_CONST] = [
    'prompt' => FALSE,
    'label' => 'type const',
    'description' => 'must exactly match the regular machine name, used in  ' .
      'code holding entity type name',
    'validation_policy' => _ENTITYSPICE_OVEN_TYPE_CONST,
    'default_policy' => _ENTITYSPICE_OVEN_TYPE_CONST,
  ];

  $tokens[_ENTITYSPICE_OVEN_MODULE_NAME] = [
    'prompt' => TRUE,
    'label' => 'module name',
    'description' => 'name of module in .info file',
    'validation_policy' => _ENTITYSPICE_OVEN_LABEL,
    'default_policy' => _ENTITYSPICE_OVEN_LABEL,
  ];
  $tokens[_ENTITYSPICE_OVEN_MODULE_DESC] = [
    'prompt' => TRUE,
    'label' => 'module description',
    'description' => 'Description of module in .info file',
    'validation_policy' => 'ascii',
    'default_policy' => _ENTITYSPICE_OVEN_MODULE_DESC,
  ];
  $tokens[_ENTITYSPICE_OVEN_MODULE_PACK] = [
    'prompt' => TRUE,
    'label' => 'module package',
    'description' => 'package to which this module belongs (In modules page) for .info file',
    'validation_policy' => _ENTITYSPICE_OVEN_LABEL,
    'default_policy' => '_ENTITYSPICE_OVEN_DEFAULT_MODULE_PACKAGE',
  ];

  $tokens[_ENTITYSPICE_OVEN_LABEL] = [
    'prompt' => TRUE,
    'label' => 'label',
    'description' => 'label of entity displayed in admin pages',
    'validation_policy' => _ENTITYSPICE_OVEN_LABEL,
    'default_policy' => _ENTITYSPICE_OVEN_LABEL,
  ];

  //  $tokens[_ENTITYSPICE_OVEN_MENU_LABEL] = [
  //    'prompt' => TRUE,
  //    'label' => 'label in menu',
  //    'description' => '',
  //    'validation_policy' => _ENTITYSPICE_OVEN_LABEL,
  //    'default_policy' => _ENTITYSPICE_OVEN_LABEL,
  //  ];
  //  $tokens[_ENTITYSPICE_OVEN_CONTENT_DEFAULT_URI] = [
  //    'prompt' => TRUE,
  //    'label' => 'default URI',
  //    'description' => 'default path from which the entity can be accessed. ' .
  //      'IMPORTANT: currently this must be a two part menu path, such as ' .
  //      'xxxx/yyyyy',
  //    'validation_policy' => 'uri',
  //    'default_policy' => '_ENTITYSPICE_OVEN_DEFAULT_URI',
  //  ];
  //  $tokens[_ENTITYSPICE_OVEN_ADMIN_BUNDLE_URI] = [
  //    'prompt' => TRUE,
  //    'label' => 'bundle administration page',
  //    'description' => 'this is a URI relative to admin/ path. Do not include admin/',
  //    'validation_policy' => 'uri',
  //    'default_policy' => '_ENTITYSPICE_OVEN_ADMIN_BUNDLE_URI',
  //  ];
  //  $tokens[_ENTITYSPICE_OVEN_DELETE_FORM_REDIRECT_URI] = [
  //    'prompt' => TRUE,
  //    'label' => 'redirect path from entity delete form',
  //    'description' => "when an entity is deleted, It's path will become " .
  //      "invalid and user should be redirected to a new path",
  //    'validation_policy' => 'uri',
  //    'default_policy' => '_ENTITYSPICE_OVEN_FORM_REDIRECT_URI',
  //  ];

  foreach (_ENTITYSPICE_OVEN_CUSTOM_PROPERTIES as $p) {
    $tokens['has_' . $p] = [
      'prompt' => TRUE,
      'label' => 'has ' . $p,
      'description' => 'whether if entity type has a ' . $p . ' property',
      'validation_policy' => 'boolean',
      'default_policy' => 'boolean_true',
    ];
    $tokens['key_' . $p] = [
      'prompt' => TRUE,
      'label' => $p . ' key',
      'description' => 'key of the property ' . $p . ' on entity, if it has one',
      'validation_policy' => _ENTITYSPICE_MACHINE,
      'default_policy' => 'key_' . $p,
    ];
  }


  $tokens['has_locked_default_bundle'] = [
    'prompt' => TRUE,
    'label' => 'has a locked default bundle',
    'description' => 'whether if entity type has a locked default bundle' .
      ' created upon module installation.',
    'validation_policy' => 'boolean',
    'default_policy' => 'boolean_false',
  ];
  $tokens['locked_default_bundle_name'] = [
    'prompt' => TRUE,
    'label' => "locked default bundle's name",
    'description' => 'what the default bundle is called if the entity type' .
      ' has any',
    'validation_policy' => _ENTITYSPICE_OVEN_MACHINE_CAMEL,
    'default_policy' => _ENTITYSPICE_OVEN_MACHINE_CAMEL,
  ];

  return $tokens;
}

// _____________________________________________________________________________

/**
 * Stolen from drush pm-download, to find where to put generated module in.
 *
 * Only used by function _entityspice_oven_pm_download_destination().
 *
 * @param $type
 * @param $drupal_root
 * @param $sitepath
 * @param bool $create
 *
 * @see \_entityspice_oven_pm_download_destination()
 * @return bool|string
 */
function _entityspice_oven_pm_download_destination_lookup($type, $drupal_root, $sitepath, $create = FALSE) {
  $destination = $type . 's';
  // Prefer /contrib if it exists.
  if ($sitepath) {
    $destination = $sitepath . '/' . $destination;
  }
  $contrib = $destination . '/contrib';
  if (is_dir($contrib)) {
    $destination = $contrib;
  }
  if ($create) {
    drush_log(dt('Attempting to create destination directory at !dir', ['!dir' => $destination]));
    drush_mkdir($destination, TRUE);
  }
  if (is_dir($destination)) {
    drush_log(dt('Using destination directory !dir', ['!dir' => $destination]));
    return $destination;
  }
  drush_log(dt('Could not find destination directory at !dir', ['!dir' => $destination]));
  return FALSE;
}

/**
 * Stolen from drush pm-download, to find where to put generated module in.
 *
 * @param $type
 *
 * @return array|bool|mixed|null|string|string[]
 */
function _entityspice_oven_pm_download_destination($type) {
  $drupal_root = drush_get_context('DRUSH_DRUPAL_ROOT');
  $site_root = drush_get_context('DRUSH_DRUPAL_SITE_ROOT');
  $full_site_root = (empty($drupal_root) || empty($site_root)) ? '' : $drupal_root . '/' . $site_root;
  $sitewide = empty($drupal_root) ? '' : $drupal_root . '/' . drush_drupal_sitewide_directory();

  $in_site_directory = FALSE;
  // Check if we are running within the site directory.
  if (strpos(realpath(drush_cwd()), realpath($full_site_root)) !== FALSE || (drush_get_option('use-site-dir', FALSE))) {
    $in_site_directory = TRUE;
  }

  $destination = '';
  // Attempt 1: If we are in a specific site directory, and the destination
  // directory already exists, then we use that.
  if (empty($destination) && $site_root && $in_site_directory) {
    $create_dir = drush_get_option('use-site-dir', FALSE);
    $destination = _entityspice_oven_pm_download_destination_lookup($type, $drupal_root, $full_site_root, $create_dir);
  }
  // Attempt 2: If the destination directory already exists for
  // the sitewide directory, use that.
  if (empty($destination) && $drupal_root) {
    $destination = _entityspice_oven_pm_download_destination_lookup($type, $drupal_root, $sitewide);
  }
  // Attempt 3: If a specific (non default) site directory exists and
  // the sitewide directory does not exist, then create destination
  // in the site specific directory.
  if (empty($destination) && $site_root && $site_root !== 'sites/default' && is_dir($full_site_root) && !is_dir($sitewide)) {
    $destination = _entityspice_oven_pm_download_destination_lookup($type, $drupal_root, $full_site_root, TRUE);
  }
  // Attempt 4: If sitewide directory exists, then create destination there.
  if (empty($destination) && is_dir($sitewide)) {
    $destination = _entityspice_oven_pm_download_destination_lookup($type, $drupal_root, $sitewide, TRUE);
  }
  // Attempt 5: If site directory exists (even default), then create
  // destination in that directory.
  if (empty($destination) && $site_root && is_dir($full_site_root)) {
    $destination = _entityspice_oven_pm_download_destination_lookup($type, $drupal_root, $full_site_root, TRUE);
  }
  // Attempt 6: If we didn't find a valid directory yet (or we somehow found
  // one that doesn't exist) we always fall back to the current directory.
  if (empty($destination) || !is_dir($destination)) {
    $destination = drush_cwd();
  }

  return $destination;
}

// _____________________________________________________________________________

function _entityspice_oven_fill_default_values($machine_name, array $tokens) {
  foreach (_entityspice_oven_tokens() as $name => $token) {
    if (!isset($tokens[$name])) {
      $tokens[$name] = _entityspice_oven_token_default_value($machine_name, $name);
    }
  };
  $tokens['machine_name'] = $machine_name;
  return $tokens;
}

/**
 * Before calling:
 *  - ensure valid machine name.
 *  - ensure no module with that machine name exist.
 *  - ensure all token values are provided.
 *  - ensure all tokens have valid values.
 *
 * Render, write files.
 *
 * After calling:
 *  - find a destination path.
 *  - move the temp directory there.
 *
 * @param array $tokens
 *
 * @return array keyed by the file name, and the (rendered) file content as
 *   string.
 */
function _entityspice_oven_render(array $tokens) {
  $files = [];
  $tokens['php'] = '<' . '?php';
  foreach (_ENTITYSPICE_OVEN_FILES as $f) {
    if ($f === 'bundle.class.inc' && !$tokens['has_bundle']) {
      continue;
    }

    ob_start();
    require "template/PLACEHOLDER.$f.tpl";
    $files[$f] = ob_get_contents();
    ob_end_clean();
  }
  return $files;
}

function _entityspice_oven_ekran($target_dir, array $tokens) {
  if (defined('DRUSH_BOOTSTRAP_DRUPAL')) {
    _entityspice_oven_ekran_drush($target_dir, $tokens);
  }
  else {
    _entityspice_oven_ekran_drupal($target_dir, $tokens);
  }
}

function _entityspice_oven_ekran_drush($target_dir, $tokens) {
  if (!drush_op('chdir', $target_dir)) {
    return 'could not chdir into target: ' . $target_dir;
  }
  foreach (_entityspice_oven_render($tokens) as $file => $text) {
    $f = drush_op('fopen', 'w', $file);
    if (!$f) {
      drush_log('warning', 'could not open file for writing, dry_run? -> ' . $file);
      continue;
    }
    drush_op('fwrite', $f, $text);
    drush_op('fclose', $f);
  }
  return '';
}

function _entityspice_oven_ekran_drupal($target_dir, $tokens) {
  if (!chdir($target_dir)) {
    return 'could not chdir into target: ' . $target_dir;
  }
  foreach (_entityspice_oven_render($tokens) as $file => $text) {
    $f = fopen($file, 'w');
    if (!$f) {
      return "could not create file: $file";
    }
    fwrite($f, $text);
    fclose($f);
  }
  return '';
}

