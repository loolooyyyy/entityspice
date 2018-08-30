<?php print $a['php'] ?>

/**
* Loads an entity by ID.
*
* @param mixed $id integer id (or any other type) uniquely identifying an
*   entity.
*
* @return \Entity the loaded entity or NULL if nothing was loaded.
*/
function <?php echo $a['machine_name'] ?>_load($id) {
  $load = entity_load("<?php echo $a['machine_name'] ?>", [$id]);
  return isset($load[$id]) ? $load[$id] : NULL;
}

/**
 * @see _entityspice_entity_load_multiple().
 */
function <?php echo $a['machine_name'] ?>_load_multiple(array $ids = [], array $conditions = [], $reset = FALSE) {
  return _entityspice_entity_load_multiple("<?php print $a['machine name'] ?>", $ids, $conditions. $reset);
}

/**
 * @see _entityspice_entity_delete().
 */
function <?php echo $a['machine_name'] ?>_delete($id) {
  return _entityspice_entity_delete("<?php print $a['machine name'] ?>", $id);
}

/**
 * @see _entityspice_entity_delete_multiple().
 */
function <?php echo $a['machine_name'] ?>_delete_multiple(array $ids) {
  return _entityspice_entity_delete_multiple("<?php print $a['machine name'] ?>", $ids);
}

/**
 * @see _entityspice_entity_save().
 */
function <?php echo $a['machine_name'] ?>_save($entity) {
  return _entityspice_entity_save("<?php print $a['machine name'] ?>", $entity);
}

/**
 * @see _entityspice_entity_view().
 */
function <?php echo $a['machine_name'] ?>_view($entity, $view_mode = 'full') {
  return _entityspice_entity_view("<?php print $a['machine name'] ?>", $entity, $view_mode);
}

/**
* Create a new entity object - DOES NOT SAVE IT.
*
* @param array $values
*   Associative array of values.
*
* @return \Entity created entity.
*/
function <?php echo $a['machine_name'] ?>_create(array $values = []) {
  return entity_get_controller("<?php echo $a['machine_name_const'] ?>")->create($values);
}

/**
 * @see _entityspice_field_extra_field_info().
 */
function <?php echo $a['machine_name'] ?>_field_extra_fields() {
  return _entityspice_field_extra_field_info("<?php print $a['machine name'] ?>");
}

/**
 * @see _entityspice_entity_access().
 */
function <?php echo $a['machine_name'] ?>_access($op, $entity = NULL, $account = NULL) {
  return _entityspice_entity_access("<?php print $a['machine name'] ?>", $op, $entity, $account);
}

/**
 * @see _entityspice_entity_info_alter().
 */
function <?php echo $a['machine_name'] ?>_entity_info_alter(&$entity_info) {
  return _entityspice_entity_info_alter("<?php print $a['machine name'] ?>", $entity_info);
}

/**
  TODO bundle?
 * @see _entityspice_entity_add_page.
 */
function <?php echo $a['machine_name'] ?>_add_page($bundle) {
  return _entityspice_entity_add_page("<?php print $a['machine name'] ?>", $bundle);
}

// ____________________________________________________________________________

/**
 * Implements hook_entity_info().
 */
function <?php echo $a['machine_name'] ?>_entity_info() {
  return _entityspice_entity_info("<?php print $a['machine name'] ?>");
}

/**
 * Makes sure entity exists.
 *
 * @param mixed $entity entity id or entity object with an id.
 *
 * @return bool true if entity exists with the given id.
 */
function _entityspice_entity_exists_in_db($entity_or_id) {
  $id = is_object($entity) ? $entity->getID() : $entity;
  // TODO!
  return entity_load("<?php echo $a['machine_name'] ?>", [$id]);
}
