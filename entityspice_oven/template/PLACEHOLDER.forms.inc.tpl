<?php print $a['php'] ?>

/**
 * @see _entityspice_entity_form().
 */
function <?php echo $a['machine_name'] ?>_form($f, &$fs, $entity) {
  $form = _entityspice_entity_form(<?php print $a['type_const'] ?>, $f, $fs, $entity);

  return $form;
}

/**
 * @see _entityspice_entity_form_validate().
 */
function <?php echo $a['machine_name'] ?>_form_validate($f, &$fs) {
  $validate = _entityspice_entity_form_validate(<?php print $a['type_const'] ?>, $f, $fs);

  return $validate;
}

/**
 * @see _entityspice_entity_form_submit().
 */
function <?php echo $a['machine_name'] ?>_form_submit($f, &$fs) {
  $submit = _entityspice_entity_form_submit(<?php print $a['type_const'] ?>, $f, $fs);

  return $submit;
}

/**
 * @see _entityspice_entity_delete_form().
 */
function <?php echo $a['machine_name'] ?>_delete_form($f, &$fs, $entity) {
  $form = _entityspice_entity_delete_form(<?php print $a['type_const'] ?>, $f, $fs, $entity);

  return $form;
}

/**
 * @see _entityspice_entity_form_entity_delete_form_submit().
 */
function <?php echo $a['machine_name'] ?>_delete_form_submit($f, &$fs) {
  $submit = _entityspice_entity_form_entity_delete_form_submit(<?php print $a['type_const'] ?>, $f, $fs);

  return $submit;
}

<?php if($a['has_bundle']): ?>
// ____________________________________________________________________________

/**
 * @see _entityspice_entity_bundle_form().
 */
function <?php echo $a['machine_name'] ?>_bundle_form($f, &$fs, $entity, $op = 'edit') {
  $form = _entityspice_entity_bundle_form(<?php print $a['type_const'] ?>, $f, $fs, $op, $entity);

  return $form;
}

/**
 * @see _entityspice_entity_bundle_form_submit().
 */
function <?php echo $a['machine_name'] ?>_bundle_form_submit(&$f, &$fs) {
  $submit = _entityspice_entity_bundle_form_submit(<?php print $a['type_const'] ?> ,$f, $fs);

  return $submit;
}

/**
 * @see _entityspice_entity_bundle_form_submit_delete().
 */
function <?php echo $a['machine_name'] ?>_form_submit_delete(&$f, &$fs) {
  $submit = _entityspice_entity_bundle_form_submit_delete(<?php print $a['type_const'] ?>, $f, $fs);

  return $submit;
}
<?php endif; ?>
