<?php print $a['php'] ?>

/**
 * @see _entityspice_entity_load().
 */
function <?php echo $a['machine_name'] ?>_load($id) {
  return _entityspice_entity_load(<?php print $a['type_const'] ?>, $id);
}

/**
 * @see _entityspice_entity_load_multiple().
 */
function <?php echo $a['machine_name'] ?>_load_multiple(array $ids = [], array $conditions = [], $reset = FALSE) {
  return _entityspice_entity_load_multiple(<?php print $a['type_const'] ?>, $ids, $conditions. $reset);
}

/**
 * @see _entityspice_entity_delete().
 */
function <?php echo $a['machine_name'] ?>_delete($id) {
  return _entityspice_entity_delete(<?php print $a['type_const'] ?>, $id);
}

/**
 * @see _entityspice_entity_delete_multiple().
 */
function <?php echo $a['machine_name'] ?>_delete_multiple(array $ids) {
  return _entityspice_entity_delete_multiple(<?php print $a['type_const'] ?>, $ids);
}

/**
 * @see _entityspice_entity_save().
 */
function <?php echo $a['machine_name'] ?>_save($entity) {
  return _entityspice_entity_save(<?php print $a['type_const'] ?>, $entity);
}

/**
 * @see _entityspice_entity_view().
 */
function <?php echo $a['machine_name'] ?>_view($entity, $view_mode = 'full') {
  return _entityspice_entity_view(<?php print $a['type_const'] ?>, $entity, $view_mode);
}

<?php if($a['has_bundle']): ?>
/**
 * @see _entityspice_entity_get_bundles().
 */
function <?php echo $a['machine_name'] ?>_get_bundles($name = NULL) {
  return _entityspice_entity_get_bundles(<?php print $a['type_const'] ?>, $name);
}

/**
 * @see _entityspice_entity_get_bundles_names().
 */
function <?php echo $a['machine_name'] ?>_get_bundles_names($with_label = FALSE) {
  return _entityspice_entity_get_bundles_names(<?php print $a['type_const'] ?>, $with_label);
}

/**
 * @see _entityspice_bundle_save().
 */
function <?php echo $a['machine_name'] ?>_bundle_save($bundle) {
  return _entityspice_bundle_save(<?php print $a['type_const'] ?>, $bundle);
}
<?php endif; ?>

/**
 * @see _entityspice_entity_create.
 */
function <?php echo $a['machine_name'] ?>_create(array $values = []) {
  return _entityspice_entity_create(<?php print $a['type_const'] ?>, $values);
}

<?php if($a['has_bundle']): ?>
/**
 * @see _entityspice_entity_get_bundles_names().
function <?php echo $a['machine_name'] ?>_bundle_options_list() {
  return _entityspice_entity_get_bundles_names(<?php print $a['type_const'] ?>, TRUE);
}

/**
 * @see _entityspice_bundle_delete().
 */
function <?php echo $a['machine_name'] ?>_bundle_delete($bundle) {
  return _entityspice_bundle_delete(<?php print $a['type_const'] ?>, $bundle);
}

/**
 * @see _entityspice_bundle_load().
 */
function <?php echo $a['machine_name'] ?>_bundle_load($bundle) {
  return _entityspice_bundle_load(<?php print $a['type_const'] ?>, $bundle);
}

/**
 * @see _entityspice_entity_bundle_access().
 */
function <?php echo $a['machine_name'] ?>_bundle_access($op, $bundle = NULL, $user = NULL) {
  return _entityspice_entity_bundle_access(<?php print $a['type_const'] ?>, $op, $bundle, $user);
}
<?php endif; ?>

/**
 * @see _entityspice_field_extra_field_info().
 */
function <?php echo $a['machine_name'] ?>_field_extra_fields() {
  return _entityspice_field_extra_field_info(<?php print $a['type_const'] ?>);
}

/**
 * @see _entityspice_entity_access().
 */
function <?php echo $a['machine_name'] ?>_access($op, $entity = NULL, $account = NULL) {
  return _entityspice_entity_access(<?php print $a['type_const'] ?>, $op, $entity, $account);
}

/**
 * @see _entityspice_entity_info_alter().
 */
function <?php echo $a['machine_name'] ?>_entity_info_alter(&$entity_info) {
  return _entityspice_entity_info_alter(<?php print $a['type_const'] ?>, $entity_info);
}

/**
 * @see _entityspice_entity_add_page.
 */
function <?php echo $a['machine_name'] ?>_add_page($bundle) {
  return _entityspice_entity_add_page(<?php print $a['type_const'] ?>, $bundle);
}

// ____________________________________________________________________________

/**
 * Implements hook_entity_info().
 */
function <?php echo $a['machine_name'] ?>_entity_info() {
  return _entityspice_entity_info(<?php print $a['type_const'] ?>);
}
