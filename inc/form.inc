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
  menu_rebuild();
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


/**
 * Get required file path which should be included with form_state.
 *
 * Modules may indicate they have no such file by setting 'has forms file'
 * property in their info hook as FALSE. The caller of this function must make
 * sure the file exists and is needed, otherwise $info['forms file'] might not
 * be set.
 *
 * @param null $etype
 *
 * @return string
 */
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

/**
 * Set required properties in $form and $form_state for entity's form.
 *
 * Every entity form needs some properties set in it's form or form_state such
 * as entity_type or entity object itself, title field and ...
 *
 * Form elements added here are:
 *   - Form include file if entity defines one.
 *   - Title field if entity has_title.
 *   - form actions element.
 *   - form validation callback.
 *   - entity fields widgets -> delegated to field API.
 *
 * @param $etype
 * @param $f
 * @param $fs
 * @param $entity
 *
 * @return mixed
 */
function _entityspice_entity_form_create_base_form($etype, $f, &$fs, $entity) {
  $info = _entityspice_get_info($etype);
  $file = _entityspice_entity_form_file_path(
    $info['has forms file'] ? $info['module'] : NULL);
  $entity = _entityspice_get_entity($etype, $entity);

  // Ensure the include file is loaded when the form is rebuilt from the cache.
  // If the entity type's module provides an include file use that otherwise
  // use our own file (it only 'might' be needed though).
  $fs['build_info']['files']['form'] = $file;

  // Add the field related form elements.
  $fs['entity'] = $entity;
  $fs['entity_type'] = $etype;

  // If entity type has a title field add it's form element.
  if ($info['has_title']) {
    $key = $info['title key'];
    $label = $info['title label'];
    $f[$key] = [
      '#type' => 'textfield',
      '#title' => t($label),
      '#required' => TRUE,
      $element['#maxlength'] = 255,
      $element['#default_value'] = $entity->{$key},
    ];
  }
  $f['#weight'] = -6;

  field_attach_form($etype, $entity, $f, $fs);

  $f['actions'] = [
    '#type' => 'container',
    '#attributes' => ['class' => ['form-actions']],
    '#weight' => 40,
  ];
  $f['#validate'][] = $info['entity form validation callback'];

  return $f;
}

/**
 * Builds an entity from user input values in form.
 *
 * Tasks done here:
 *   - Set entity UID.
 *   - Set entity title if entity type defines title field.
 *   - Call field_attach_submit.
 *
 * @param $etype
 * @param $f
 * @param $fs
 *
 * @return mixed
 */
function _entityspice_entity_form_build_entity_from_form($etype, $f, &$fs) {
  $entity = &$fs['entity'];
  $v = $fs['values'];
  $info = _entityspice_get_info($etype);

  $entity->uid = $GLOBALS['user']->uid;
  if ($info->hasTitle()) {
    $entity->{$info->titleKey()} = $v[$info->titleKey()];
  }
  field_attach_submit($etype, $entity, $f, $fs);
  return $entity;
}

/**
 * Set page redirect to $entity view page.
 *
 * Check if entity has ID, which means it is successfully saved, if not,
 * only rebuild the form without redirecting to a new page.
 *
 * Then if entity is successfully saved:
 * If user has permission "view entity of type $etype", sets page redirect to
 * $entity view page. If not, sets redirect to front page.
 *
 * @param $etype
 * @param $fs
 * @param $entity
 *
 * @return bool
 */
function _entityspice_entity_form_set_redirect_to_entity($etype, &$fs, $entity) {
  if ($id = $entity->getId()) {
    $info = entity_get_info($etype);
    $idkey = $info['entity keys']['id'];

    $fs['values']['eid'] = $id;
    $fs['values'][$idkey] = $id;
    $fs['eid'] = $id;
    $fs[$idkey] = $id;
    $access = entity_access('view', $etype, $entity);
    $uri = entity_uri($etype, $entity);
    $fs['redirect'] = $access ? $uri['path'] : '<front>';
    return TRUE;
  }
  else {
    $fs['rebuild'] = TRUE;
    return FALSE;
  }
}

// ___________________________________________________________________________

/**
 * Generates base form for creating/editing entities.
 *
 * @param $etype
 * @param $f
 * @param $fs
 * @param $entity
 *
 * @return mixed
 */
function _entityspice_entity_form($etype, $f, &$fs, $entity) {
  $scb = [
    _entityspice_get_info($etype)->entityFormSubmitCallback(),
  ];
  $f = _entityspice_entity_form_create_base_form($etype, $f, $fs, $entity);

  // We add the form's #submit array to this button along with the actual submit
  // handler to preserve any submit handlers added by a form callback_wrapper.
  if (empty($fs['#submit'])) {
    $fs['#submit'] = [];
  }
  $f['actions']['submit'] = [
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array_merge($scb, $fs['#submit']),
  ];

  return $f;
}

/**
 * Validation callback for entity add/edit form.
 *
 * @param $etype
 * @param $f
 * @param $fs
 */
function _entityspice_entity_form_validate($etype, $f, &$fs) {
  $entity = $fs['entity'];
  // Notify field widgets to validate their data.
  field_attach_form_validate($etype, $entity, $f, $fs);
}

/**
 * Submit callback for entity add/edit form.
 *
 * @param $etype
 * @param $f
 * @param $fs
 */
function _entityspice_entity_form_submit($etype, $f, &$fs) {
  $info = _entityspice_get_info($etype);
  $entity = _entityspice_entity_form_build_entity_from_form($etype, $f, $fs);
  $entity = entity_save($etype, $entity);
  if ($info->hasTitle()) {
    $entity->{$info->titleKey()} = $fs['values'][$info->titleKey()];
  }
  $name = _entityspice_get_entity_name($etype, $entity);
  $msg = t('@entity saved.', ['@entity' => $name]);
  drupal_set_message($msg, 'status', FALSE);
  _entityspice_entity_form_set_redirect_to_entity($etype, $fs, $entity);
}

/**
 * Form callback: confirmation form for deleting an entity.
 *
 * @see confirm_form()
 *
 * @param $etype
 * @param $f
 * @param $fs
 * @param $entity
 *
 * @return mixed|null
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
