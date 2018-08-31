<?php

// ______________________________________________________________________ ADMIN

// TODO node admin page but with views.

// _______________________________________________________________ BUNDLE ADMIN

// TODO doesn't entity api do already?

/**
 * Generates the entity bundle editing form.
 *
 * @param $etype
 * @param $f
 * @param $fs
 * @param $op
 * @param $bundle_entity
 *
 * @return mixed
 */
function _entityspice_entity_bundle_form($etype, $f, &$fs, $op, $bundle_entity) {
  $info = _entityspice_get_info($etype);
  $exists_callback = $info->bundleExistsCallback();;

  if ($op == 'clone') {
    $bundle_entity->label .= ' (cloned)';
    $bundle_entity->name = '';
  }

  $f['#base_type'] = $etype;
  $f['#bundle_entity'] = $bundle_entity;
  $fs['bundle_entity'] = $bundle_entity;

  $f['label'] = [
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $bundle_entity->label,
    '#description' => t('Human-readable name of this type.'),
    '#required' => TRUE,
    '#size' => 30,
  ];
  $f['name'] = [
    '#type' => 'machine_name',
    '#default_value' => isset($bundle_entity->name) ? $bundle_entity->name : '',
    '#maxlength' => 32,
    '#disabled' => $bundle_entity->locked || $op == 'edit',
    '#machine_name' => [
      'exists' => $exists_callback,
      'source' => ['label'],
    ],
    '#description' => t('A unique machine-readable name for this bundle. It must only contain lowercase letters, numbers, and underscores.'),
  ];
  $f['actions'] = ['#type' => 'actions'];
  $f['actions']['submit'] = [
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 40,
  ];

  if (!$bundle_entity->locked && $op == 'edit') {
    $f['actions']['delete'] = [
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#weight' => 45,
      '#limit_validation_errors' => [],
      '#submit' => [$info['bundle form submit delete']],
    ];
  }

  return $f;
}

/**
 * Form API submit callback for the bundle form.
 *
 * @param $etype
 * @param $f
 * @param $fs
 */
function _entityspice_entity_bundle_form_submit($etype, &$f, &$fs) {
  $bundle_entity = entity_ui_form_submit_build_entity($f, $fs);
  $bundle_entity->save();
  $fs['redirect'] = _entityspice_get_info($etype)->bundleSubmitRedirect();
}

/**
 * Form API submit callback for the bundle delete button.
 *
 * @param $etype
 * @param $fs
 */
function _entityspice_entity_bundle_form_submit_delete($etype, &$fs) {
  $r = _entityspice_get_info($etype)->parentAdminPath();
  $r .= '/manage/' . $fs['bundle_entity']->name . '/delete';
  $fs['redirect'] .= $r;
}

// _____________________________________________________________________ ENTITY

function _entityspice_entity_form_file_path($etype = NULL) {
  if ($etype) {
    $info = _entityspice_get_info($etype);
    $file = $info->formsFile();
    return drupal_get_path('module', $info['module']) . '/' . $file;
  }
  else {
    return drupal_get_path('module', 'entityspice') . '/entity/entity_forms.inc';
  }
}

// ___________________________________________________________________________

/**
 * Submit callback for entity add/edit form.
 */
function _entityspice_entity_form_submit($f, &$fs) {
  $machine_name = NULL;
  $entity = &$fs['entity'];

  // TODO custom prop

  field_attach_submit($machine_name, $entity, $f, $fs);
  $entity = entity_save($machine_name, $entity);

  $msg = t('Saved.');
  drupal_set_message($msg, 'status', FALSE);

  // TODO!!!
  if ($id = $entity->getId()) {
    $info = entity_get_info($machine_name);
    $idkey = $info['entity keys']['id'];

    $fs['values']['eid'] = $id;
    $fs['values'][$idkey] = $id;
    $fs['eid'] = $id;
    $fs[$idkey] = $id;
    $access = entity_access('view', $machine_name, $entity);
    $uri = entity_uri($machine_name, $entity);
    $fs['redirect'] = $access ? $uri['path'] : '<front>';
    return TRUE;
  }
  else {
    $fs['rebuild'] = TRUE;
    return FALSE;
  }
}

/**
 * Form callback: confirmation form for deleting an entity.
 */
function _entityspice_entity_delete_form($etype, $f, &$fs, $entity) {
  if (!_entityspice_entity_in_path_exists($etype, $entity)) {
    drupal_not_found();
    return NULL;
  }

  $info = _entityspice_get_info($etype);

  $scb = $info->entityDeleteFormSubmitCallback();
  $entity = _entityspice_get_entity($etype, $entity);

  $fs['entity'] = $entity;
  $fs['entity_type'] = $etype;

  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $fs['build_info']['files']['form'] = _entityspice_entity_form_file_path(
    $info->hasFormsFile() ? $info->module() : NULL);

  $f['#submit'][] = $scb;

  $name = _entityspice_get_entity_name($etype, $entity);
  $targs = ['@title' => $name];
  $msg = t('Are you sure you want to delete entity @title?', $targs);
  return confirm_form($f,
    $msg,
    entity_uri($etype, $entity),
    '<p>' . t('Deleting this entity cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );
}

/**
 * Submit callback for entity delete form.
 *
 * @param $etype
 * @param $fs
 */
function _entityspice_entity_form_entity_delete_form_submit($etype, &$fs) {
  $entity = $fs['entity'];
  $name = _entityspice_get_entity_name($etype, $entity);

  $targ = ['@name' => $name];
  if (_entityspice_entity_delete($etype, $entity->getId())) {
    drupal_set_message(t('@name has been deleted.', $targ));
    watchdog($etype, 'Deleted entity @name.', $name, WATCHDOG_NOTICE);
    $fs['redirect'] = _entityspice_get_info($etype)->entityDeleteRedirect();
  }
  else {
    drupal_set_message(t('@name could not be deleted.', $targ), 'error');
  }
}
