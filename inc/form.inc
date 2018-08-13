<?php

// ______________________________________________________________________ ADMIN

/**
 * Returns the form to filter entity lists.
 *
 * Original code in Redhen module.
 */
function entityspice_filter_form($f, &$fs, $etype) {
  drupal_add_library('system', 'ui.datepicker');
  drupal_add_js(drupal_get_path('module', 'entityspice') . '/entityspice.forms.js');

  $property_info = entity_get_all_property_info($etype);
  if (!empty($property_info)) {
    $f['filter_fieldset'] = [
      '#type' => 'fieldset',
      '#title' => t('Filters'),
      '#collapsible' => TRUE,
      '#collapsed' => (empty($_GET['properties']) && empty($_GET['fields']) && empty($_GET['bundle'])),
    ];

    $f['filter_fieldset']['filters'] = [
      '#type' => 'container',
      '#tree' => TRUE,
    ];

    $f['filter_fieldset']['filters']['properties'] = [
      '#type' => 'container',
    ];

    // Add a form field for each property.
    foreach ($property_info as $key => $prop) {
      if (isset($prop['filter']) && $prop['filter']) {
        $field_type = isset($prop['field_type']) ? $prop['field_type'] : $prop['type'];

        // Convert datetime defaults to date strings.
        $default_value = isset($_GET['properties'][$key]) ? $_GET['properties'][$key] : NULL;
        if ($prop['type'] == 'date' && !empty($default_value)) {
          $default_value['from'] = ($default_value['from'] == '') ? '' :
            format_date(strtotime($default_value['from']), 'medium', 'm/d/Y');
          $default_value['to'] = ($default_value['to'] == '') ? '' :
            format_date(strtotime($default_value['to']), 'medium', 'm/d/Y');
        }

        if ($prop['type'] == 'date') {
          $f['filter_fieldset']['filters']['properties'][$key]['from'] = [
            '#type' => $field_type,
            '#title' => t('@label from', ['@label' => $prop['label']]),
            '#attributes' => ['class' => ["entityspice-field-type-{$prop['type']}"]],
            '#default_value' => $default_value['from'],
          ];
          $f['filter_fieldset']['filters']['properties'][$key]['to'] = [
            '#type' => $field_type,
            '#title' => t('@label to', ['@label' => $prop['label']]),
            '#attributes' => ['class' => ["entityspice-field-type-{$prop['type']}"]],
            '#default_value' => $default_value['to'],
          ];
        }
        else {
          $f['filter_fieldset']['filters']['properties'][$key] = [
            '#type' => $field_type,
            '#title' => check_plain($prop['label']),
            '#attributes' => ['class' => ["entityspice-field-type-{$prop['type']}"]],
            '#default_value' => $default_value,
          ];
        }

        if ($field_type == 'select' && isset($prop['options list'])) {
          $select_options = ['' => t('-- Any --')] + call_user_func($prop['options list']);
          $f['filter_fieldset']['filters']['properties'][$key]['#options'] = $select_options;
        }
      }
    }

    // Add bundles; will load additional field filters on change.
    $type_options = ['' => t('-- Any --')];
    $entity_info = entity_get_info($etype);
    foreach ($entity_info['bundles'] as $key => $bundle) {
      $type_options[$key] = $bundle['label'];
    }

    // Single bundle, just load fields.
    if (count($entity_info['bundles']) == 1) {
      $bundle = array_keys($entity_info['bundles']);
      $bundle = $bundle[0];
      _entityspice_filter_form_fields($etype, $f, $fs, $bundle);
    }
    else {
      $f['filter_fieldset']['filters']['bundle'] = [
        '#type' => 'select',
        '#title' => t('@type types', ['@type' => $entity_info['label']]),
        '#options' => $type_options,
        '#default_value' => isset($_GET['bundle']) ? $_GET['bundle'] : NULL,
        '#ajax' => [
          'callback' => '_entityspice_filter_fields_callback',
          'wrapper' => 'entityspice_filter_fields_container',
          'method' => 'replace',
          'effect' => 'fade',
          'progress' => [
            'type' => 'throbber',
            'message' => t('Retrieving fields for this @entity_type type.',
              ['@entity_type' => $entity_info['label']]),
          ],
        ],
      ];
      $f['filter_fieldset']['filters']['fields'] = [
        '#prefix' => '<div id="entityspice_filter_fields_container">',
        '#suffix' => '</div>',
      ];
    }

    $f['filter_fieldset']['submit'] = [
      '#type' => 'submit',
      '#value' => t('Apply'),
    ];

    $f['filter_fieldset']['reset'] = [
      '#type' => 'submit',
      '#value' => t('Reset'),
      '#limit_validation_errors' => [],
      '#submit' => ['entityspice_filter_form_reset'],
    ];
  }

  // Add the fields on an ajax type change event or if a filter is being passed.
  if (!empty($fs['values']['filters']['bundle']) || isset($_GET['bundle'])) {
    $bundle = !empty($fs['values']['filters']['bundle']) ?
      $fs['values']['filters']['bundle'] : $_GET['bundle'];

    _entityspice_filter_form_fields($etype, $f, $fs, $bundle);
  }

  return $f;
}

/**
 * AJAX callback to return fields for a given type.
 */
function _entityspice_filter_fields_callback($f, $fs) {
  return $f['filter_fieldset']['filters']['fields'];
}

/**
 * Validation callback for filter form.
 */
function entityspice_filter_form_validate($f, &$fs) {
  // @TODO: Date field validation.
}

/**
 * Submit handler for entityspice_sec_filter_form().
 */
function entityspice_filter_form_submit($f, &$fs) {
  $filters = $fs['values']['filters'];
  $fs['redirect'] = [$_GET['q'], ['query' => $filters]];
}

/**
 * Submit handler to reset the filter form.
 */
function entityspice_filter_form_reset($f, &$fs) {
  $fs['redirect'] = [$_GET['q']];
}

/**
 * Populate a form array with field filters for given bundle.
 *
 * @param array $f
 *   Form to alter.
 * @param array $fs
 *   Form state to alter.
 * @param string $etype
 *   Entity type.
 * @param string $bundle
 *   Bundle.
 */
function _entityspice_filter_form_fields($etype, &$f, &$fs, $bundle) {
  $fields = field_info_instances($etype, $bundle);
  $f['filter_fieldset']['filters']['fields'] = [
    '#id' => 'entityspice_filter_fields_container',
  ];
  // If there are multiple bundle types, show which one is current.
  if (isset($f['filter_fieldset']['filters']['bundle'])) {
    $f['filter_fieldset']['filters']['fields']['#type'] = 'fieldset';
    $f['filter_fieldset']['filters']['fields']['#title'] = t('Filter by %bundle specific fields',
      ['%bundle' => $bundle]
    );
  }
  else {
    $f['filter_fieldset']['filters']['fields']['#type'] = 'container';
  }
  $filters_count = 0;
  if (!empty($fields)) {
    if (!isset($fs['field'])) {
      $fs['field'] = [];
    }

    // Loop through each field adding a filter.
    $filters_count = 0;
    foreach ($fields as $field => $instance) {
      $field_info = field_info_field($instance['field_name']);

      $filters_count++;

      // Insert the widget.
      // field_default_form() assumes the $f has a #parents property.
      $element = [
        '#parents' => [],
      ];
      // Because of the redirect in entityspice_filter_form_submit(), $fs
      // has no values. Instead, read directly from $_GET.
      $items = isset($_GET['fields'][$field]) ? $_GET['fields'][$field] : [LANGUAGE_NONE => []];
      $instance['required'] = FALSE;
      $instance['description'] = '';
      $field_info['cardinality'] = 1;

      // Set up specific field types.
      switch ($field_info['type']) {
        // Set up date fields.
        case 'date':
        case 'datestamp':
        case 'datetime':
          // Ensure we have a 'to' date.
          $field_info['settings']['todate'] = 'required';
          // In case date repeat is present, disable it.
          $field_info['settings']['repeat'] = FALSE;
          break;
      }

      // The last argument to field_default_form() fetches a single delta.
      $additions = field_default_form($etype, NULL, $field_info, $instance, LANGUAGE_NONE, $items[LANGUAGE_NONE], $element, $fs, 0);

      foreach ($additions as $field_name => $addition) {
        // TODO: Handle country ajax post back. It currently corrupts the form.
        if ($field_info['type'] == 'addressfield') {
          // This hack sets up the form with the default country.
          $addition[LANGUAGE_NONE][0]['country']['#access'] = FALSE;
        }

        // Append an explanation on the handling of boolean values.
        if ($field_info['type'] == 'list_boolean') {
          $addition[LANGUAGE_NONE]['#type'] = 'radios';
          $addition[LANGUAGE_NONE]['#options'] = [
            0 => t('False'),
            1 => t('True'),
            'skip' => t('Skip'),
          ];
          $addition[LANGUAGE_NONE]['#description'] = $addition[LANGUAGE_NONE]['#description'] .
            t('This boolean checkbox has been converted to radio buttons to allow filtering for false as well as skipping non-required fields. .');
        }

        $f['filter_fieldset']['filters']['fields'][$field_name] = $addition;
      }
    }
  }
  // Show a message if there is more than one bundle and this one has no
  // applicable fields.
  if (!$filters_count && isset($f['filter_fieldset']['filters']['bundle'])) {
    $f['filter_fieldset']['filters']['fields']['message'] = [
      '#markup' => t('There are no additional fields for %bundle.', ['%bundle' => $bundle]),
    ];
  }
}

// _______________________________________________________________ BUNDLE ADMIN

/**
 * Generates the entity bundle editing form.
 */
function _entityspice_entity_bundle_form($etype, $f, &$fs, $op, $bundle_entity, array $overrides = []) {
  $info = _entityspice_get_info_override($etype, $overrides);
  $exists_callback = $info['bundle exists callback'];;

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
    '#description' => t('The human-readable name of this type.'),
    '#required' => TRUE,
    '#size' => 30,
  ];
  // Machine-readable type name.
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
 */
function _entityspice_entity_bundle_form_submit($etype, &$f, &$fs, array $overrides = []) {
  $bundle_entity = entity_ui_form_submit_build_entity($f, $fs);
  $bundle_entity->save();
  menu_rebuild();
  $fs['redirect'] = _entityspice_get_info_override($etype, $overrides)['bundle submit redirect'];
}

/**
 * Form API submit callback for the bundle delete button.
 */
function _entityspice_entity_bundle_form_submit_delete($etype, &$f, &$fs, array $overrides = []) {
  if (isset($overrides['_redirect'])) {
    $r = $overrides['_redirect'];
  }
  else {
    $r = _entityspice_get_info_override($etype, $overrides)['parent admin path'];
    $r .= '/manage/' . $fs['bundle_entity']->name . '/delete';
  }

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
 */
function _entityspice_entity_form_file_path($etype = NULL) {
  // If a file should be included
  if ($etype) {
    $info = _entityspice_get_info($etype);
    $file = $info['forms file'];
    $file = $file['directory'] . $file['file'] . '.' . $file['suffix'];

    return drupal_get_path('module', $info['module']) . '/' . $file;
  }

  return drupal_get_path('module', 'entityspice') . '/entity/entity_forms.inc';
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
 */
function _entityspice_entity_form_create_base_form($etype, $f, &$fs, $entity, array $overrides = []) {
  $info = _entityspice_get_info_override($etype, $overrides);
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
 */
function _entityspice_entity_form_build_entity_from_form($etype, $f, &$fs, array $overrides = []) {
  $entity = &$fs['entity'];
  $v = $fs['values'];
  $info = _entityspice_get_info_override($etype, $overrides);

  $entity->uid = $GLOBALS['user']->uid;
  if ($info['has_title']) {
    $entity->{$info['title key']} = $v[$info['title key']];
  }

  // In case calling function does not want this hook fired they can set this
  // to FALSE.
  // Notify field widgets.
  if (!isset($overrides['_field submit']) || $overrides['_field submit']) {
    field_attach_submit($etype, $entity, $f, $fs);
  }

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
 */
function _entityspice_entity_form_set_redirect_to_entity($etype, &$fs, $entity, array $overrides = []) {
  if ($id = $entity->getID()) {
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
 */
function _entityspice_entity_form($etype, $f, &$fs, $entity, array $overrides = []) {
  $scb = [
    _entityspice_get_info_override($etype, $overrides)['entity form submit callback'],
  ];
  $f = _entityspice_entity_form_create_base_form($etype, $f, $fs, $entity, $overrides);

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
 */
function _entityspice_entity_form_validate($etype, $f, &$fs) {
  $entity = $fs['entity'];

  // Notify field widgets to validate their data.
  field_attach_form_validate($etype, $entity, $f, $fs);
}

/**
 * Submit callback for entity add/edit form.
 */
function _entityspice_entity_form_submit($etype, $f, &$fs, array $overrides = []) {
  $info = _entityspice_get_info_override($etype, $overrides);
  $entity = _entityspice_entity_form_build_entity_from_form($etype, $f, $fs, $overrides);

  // Save the entity.
  $entity = entity_save($etype, $entity);

  if ($info['has_title']) {
    $key = $info['title key'];
    $entity->{$key} = $fs['values'][$key];
  }

  $name = _entityspice_get_entity_name($etype, $entity, $overrides);
  $msg = t('@entity saved.', ['@entity' => $name]);
  drupal_set_message($msg, 'status', FALSE);

  _entityspice_entity_form_set_redirect_to_entity($etype, $fs, $entity, $overrides);
}

/**
 * Form callback: confirmation form for deleting an entity.
 *
 * @see confirm_form()
 */
function _entityspice_entity_delete_form($etype, $f, &$fs, $entity, array $overrides = []) {
  if (!_entityspice_entity_in_path_exists($etype, $entity)) {
    return drupal_not_found();
  }

  $scb = _entityspice_get_info_override($etype, $overrides)['entity delete form submit callback'];
  $entity = _entityspice_get_entity($etype, $entity);
  $info = _entityspice_get_info_override($etype, $overrides);

  $fs['entity'] = $entity;
  $fs['entity_type'] = $etype;

  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $fs['build_info']['files']['form'] = _entityspice_entity_form_file_path(
    $info['has forms file'] ? $info['module'] : NULL);

  $f['#submit'][] = $scb;

  $name = _entityspice_get_entity_name($etype, $entity, $overrides);
  $targs = ['@title' => $name];
  $msg = t('Are you sure you want to delete entity @title?', $targs);

  $f = confirm_form($f,
    $msg,
    entity_uri($etype, $entity),
    '<p>' . t('Deleting this entity cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $f;
}

/**
 * Submit callback for entity delete form.
 */
function _entityspice_entity_form_entity_delete_form_submit($etype, $f, &$fs, array $overrides = []) {
  $entity = $fs['entity'];
  $name = _entityspice_get_entity_name($etype, $entity, $overrides);

  $targ = ['@name' => $name];
  if (_entityspice_entity_delete($etype, $entity->getID())) {
    drupal_set_message(t('@name has been deleted.', $targ));
    watchdog($etype, 'Deleted entity @name.', $name, WATCHDOG_NOTICE);
    $fs['redirect'] = _entityspice_get_info_override($etype, $overrides)['entity delete redirect'];
  }
  else {
    drupal_set_message(t('@name could not be deleted.', $targ), 'error');
  }
}
