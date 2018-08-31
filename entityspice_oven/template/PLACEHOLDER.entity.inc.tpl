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
  $machine_name = '<?php echo $a['machine_name'] ?>';
  $load = entity_load($machine_name, [$id]);
  return isset($load[$id]) ? $load[$id] : NULL;
}

/**
 * @see entity_load().
 */
function <?php echo $a['machine_name'] ?>_load_multiple(array $ids = [], array $conditions = [], $reset = FALSE) {
  $machine_name = '<?php echo $a['machine_name'] ?>';
  return entity_load($machine_name, $ids, $conditions, $reset);
}

/**
 * @see _entityspice_entity_delete().
 */
function <?php echo $a['machine_name'] ?>_delete($id) {
  return <?php echo $a['machine_name'] ?>_delete_multiple([$id]);
}

/**
 * Deletes multiple entities by ID.
 */
function <?php echo $a['machine_name'] ?>_delete_multiple(array $ids) {
  $machine_name = '<?php echo $a['machine_name'] ?>';
  return entity_get_controller($machine_name)->delete($ids);
}

/**
 * Saves an entity.
 */
function <?php echo $a['machine_name'] ?>_save($entity) {
  $entity->save();
  return $entity;
}

/**
* Display an entity.
*/
function <?php echo $a['machine_name'] ?>_view($entity, $view_mode = 'full') {
  $machine_name = '<?php echo $a['machine_name'] ?>';
  if ($entity = _entityspice_get_entity($machine_name, $entity)) {
    return $entity->view($view_mode);
  }
  drupal_not_found();
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
  $machine_name = '<?php echo $a['machine_name'] ?>';
  return entity_get_controller($machine_name)->create($values);
}

/**
 * @see _entityspice_field_extra_field_info().
 */
function <?php echo $a['machine_name'] ?>_field_extra_fields() {
  $machine_name = '<?php echo $a['machine_name'] ?>';
  return _entityspice_field_extra_field_info($machine_name);
}

/**
 * @see _entityspice_entity_access().
 */
function <?php echo $a['machine_name'] ?>_access($op, $entity = NULL, $account = NULL) {
  $machine_name = '<?php echo $a['machine_name'] ?>';
  return _entityspice_entity_access($machine_name, $op, $entity, $account);
}

/**
 * @see _entityspice_entity_info_alter().
 */
function <?php echo $a['machine_name'] ?>_entity_info_alter(&$entity_info) {
  $machine_name = '<?php echo $a['machine_name'] ?>';
  return _entityspice_entity_info_alter($machine_name, $entity_info);
}

/**
  TODO bundle?
 * @see _entityspice_entity_add_page.
 */
function <?php echo $a['machine_name'] ?>_add_page($bundle) {
  $machine_name = '<?php echo $a['machine_name'] ?>';
  return _entityspice_entity_add_page($machine_name, $bundle);
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
