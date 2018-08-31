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
  $submit = _entityspice_entity_form_submit($machine_name, $f, $fs);
  return $submit;
}

/**
 * @see _entityspice_entity_delete_form().
 */
function <?php echo $a['machine_name'] ?>_delete_form($f, &$fs, $entity) {
  $machine_name = '<?php echo $a['machine_name'] ?>';
  $form = _entityspice_entity_delete_form($machine_name, $f, $fs, $entity);

  return $form;
}

/**
 * @see _entityspice_entity_form_entity_delete_form_submit().
 */
function <?php echo $a['machine_name'] ?>_delete_form_submit($f, &$fs) {
  $machine_name = '<?php echo $a['machine_name'] ?>';
  $submit = _entityspice_entity_form_entity_delete_form_submit($machine_name, $f, $fs);
  return $submit;
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
