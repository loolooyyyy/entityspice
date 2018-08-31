<?php print $a['php'] ?>

/**
 * Base form for creating/editing entities.
 */
function <?php echo $a['machine_name'] ?>_form($f, &$fs, $entity) {
  $machine_name = '<?php echo $a['machine_name'] ?>';
  $module = '<?php echo $a['module'] ?>';
  $validation_callback = '<?php echo $a['safe'] . $a['machine_name'] ?>_form_validate';
  $submit_callback = '<?php echo $a['safe'] . $a['machine_name'] ?>_form_submit';

  $fs['build_info']['files']['form'] = drupal_get_path(
      'module', $module) . '/' . $module . '.forms.inc';

  $fs['entity'] = $entity;
  $fs['entity_type'] = $machine_name;

  // TODO CUSTOM PROP HERE

  field_attach_form($machine_name, $entity, $f, $fs);

  $f['actions'] = [
    '#type' => 'container',
    '#attributes' => ['class' => ['form-actions']],
    '#weight' => 40,
  ];
  $f['#validate'][] = $validation_callback;

  // TODO hmm?
  // We add the form's #submit array to this button along with the actual submit
  // handler to preserve any submit handlers added by a form callback_wrapper.
  if (empty($fs['#submit'])) {
    $fs['#submit'] = [];
  }
  $f['actions']['submit'] = [
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array_merge([$submit_callback], $fs['#submit']),
  ];

  return $f;
}

/**
 * Validation callback for entity add/edit form.
 */
function <?php echo $a['safe'] . $a['machine_name'] ?>_form_validate($f, &$fs) {
  $machine_name = '<?php echo $a['machine_name'] ?>';
  $entity = $fs['entity'];
  field_attach_form_validate($machine_name, $entity, $f, $fs);
}

/**
 * @see _entityspice_entity_form_submit().
 */
function <?php echo $a['safe'] . $a['machine_name'] ?>_form_submit($f, &$fs) {
  $machine_name = '<?php echo $a['machine_name'] ?>';
  $entity = &$fs['entity'];

  // TODO custom prop

  field_attach_submit($machine_name, $entity, $f, $fs);
  $entity = entity_save($machine_name, $entity);

  $name = <?php echo $a['safe'] . $a['machine_name'] ?>_entity_name($entity);
  $msg = t('@name saved.', ['@name' => $name]);
  drupal_set_message($msg, 'status', FALSE);

  $submit = _entityspice_entity_form_submit($machine_name, $f, $fs);
  return $submit;
}

/**
 * Form callback: confirmation form for deleting an entity.
 */
function <?php echo $a['safe'] . $a['machine_name'] ?>_delete_form($f, &$fs, $entity) {
  $module = '<?php echo $a['module'] ?>';
  $machine_name = '<?php echo $a['machine_name'] ?>';
  $submit_callback = '<?php echo $a['safe'] . $a['machine_name'] ?>_delete_form_submit';

  if (!_entityspice_entity_in_path_exists($machine_name, $entity)) {
    return drupal_not_found();
  }

  $fs['entity'] = $entity;
  $fs['entity_type'] = $machine_name;
  $fs['build_info']['files']['form'] = drupal_get_path('module', $module) . '/' . $module . 'forms.inc';

  $f['#submit'][] = $submit_callback;

  $name = <?php echo $a['safe'] . $a['machine_name'] ?>_entity_name($entity);
  $msg = t('Are you sure you want to delete entity @title?', ['@title' => $name]);
  return confirm_form($f,
    $msg,
    entity_uri($machine_name, $entity),
    '<p>' . t('Deleting this entity cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );
}

/**
 * Submit callback for entity delete form.
 */
function <?php echo $a['safe'] . $a['machine_name'] ?>_delete_form_submit($f, &$fs) {
  $machine_name = '<?php echo $a['machine_name'] ?>';
  $redirect = <?php echo $a['entity delete redirect']; ?>

  $entity = $fs['entity'];
  $id = <?php echo $a['safe'] . $a['machine_name'] ?>_entity_id($entity);
  $name = <?php echo $a['safe'] . $a['machine_name'] ?>_entity_name($entity);

  if(<?php echo $a['machine_name'] ?>_delete($id)) {
    drupal_set_message(t('@name has been deleted.', ['@name' => $name]));
    watchdog($etype, 'deleted entity @name.', $name, WATCHDOG_NOTICE);
    $fs['redirect'] = $redirect;
  }
  else {
    drupal_set_message(t('@name could not be deleted.', ['@name' => $name]), 'error');
  }
}

<?php if($a['has_bundle']): ?>
// ____________________________________________________________________________

/**
 * @see _entityspice_entity_bundle_form().
 */
function <?php echo $a['machine_name'] ?>_bundle_form($f, &$fs, $entity, $op = 'edit') {
  $machine_name = '<?php echo $a['machine_name'] ?>';
  $form = _entityspice_entity_bundle_form($machine_name, $f, $fs, $op, $entity);
  return $form;
}

/**
 * @see _entityspice_entity_bundle_form_submit().
 */
function <?php echo $a['machine_name'] ?>_bundle_form_submit(&$f, &$fs) {
  $machine_name = '<?php echo $a['machine_name'] ?>';
  $submit = _entityspice_entity_bundle_form_submit($machine_name ,$f, $fs);
  return $submit;
}

/**
 * @see _entityspice_entity_bundle_form_submit_delete().
 */
function <?php echo $a['machine_name'] ?>_form_submit_delete(&$f, &$fs) {
  $machine_name = '<?php echo $a['machine_name'] ?>';
  $submit = _entityspice_entity_bundle_form_submit_delete($machine_name, $f, $fs);
  return $submit;
}
<?php endif; ?>
