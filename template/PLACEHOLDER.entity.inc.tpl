<?php print $a->php->value; ?>

/**
* Loads an entity by ID.
*
* @param mixed $id integer id (or any other type) uniquely identifying an
*   entity.
*
* @return \Entity the loaded entity or NULL if nothing was loaded.
*/
function <?php echo $a->m; ?>_load($id) {
  $machine_name = '<?php echo $a->m; ?>';
  $load = entity_load($machine_name, [$id]);
  return isset($load[$id]) ? $load[$id] : NULL;
}

/**
 * @see entity_load().
 */
function <?php echo $a->m; ?>_load_multiple(array $ids = [], array $conditions = [], $reset = FALSE) {
  $machine_name = '<?php echo $a->m; ?>';
  return entity_load($machine_name, $ids, $conditions, $reset);
}

/**
 * Deletes an entity by ID.
 */
function <?php echo $a->m; ?>_delete($id) {
  return <?php echo $a->m; ?>_delete_multiple([$id]);
}

/**
 * Deletes multiple entities by ID.
 */
function <?php echo $a->m; ?>_delete_multiple(array $ids) {
  $machine_name = '<?php echo $a->m; ?>';
  return entity_get_controller($machine_name)->delete($ids);
}

/**
 * Saves an entity.
 */
function <?php echo $a->m; ?>_save($entity) {
  $entity->save();
  return $entity;
}

/**
* Display an entity.
*/
function <?php echo $a->m; ?>_view($entity, $view_mode = 'full') {
  return $entity->view($view_mode);
}

/**
* Create a new entity object - DOES NOT SAVE IT.
*
* @param array $values
*   Associative array of values.
*
* @return \Entity created entity.
*/
function <?php echo $a->m; ?>_create(array $values = []) {
  $machine_name = '<?php echo $a->m; ?>';
  return entity_get_controller($machine_name)->create($values);
}

/**
 * Implements hook_field_extra_field_info().
 */
function <?php echo $a->m; ?>_field_extra_fields() {
  // TODO custom prop
  $machine_name = '<?php echo $a->m; ?>';
  $extra = [];
// $bundles = NULL;
// foreach ($bundles as $bundle => $info) {
//   $extra[$machine_name][$bundle] = [
//     'form' => [
//       'title' => [
//           'label' => t('Title'),
//           'description' => t('Entity title'),
//           'weight' => 0,
//         ] + $form,
//     ],
//     'display' => [
//         'title' => [
//           'label' => t('Title'),
//           'weight' => 0,
//         ],
//       ] + $display,
//   ];
// }
  return $extra;
}

/**
 */
function <?php echo $a->m; ?>_access($op, $entity = NULL, $account = NULL) {
  $machine_name = '<?php echo $a->m; ?>';
  return _entityspice_entity_access($machine_name, $op, $entity, $account);
}

/**
 */
function <?php echo $a->m; ?>_entity_info_alter(&$entity_info) {
  $machine_name = '<?php echo $a->m; ?>';
  return _entityspice_entity_info_alter($machine_name, $entity_info);
}

/**
  TODO bundle?
 */
function <?php echo $a->m; ?>_add_page($bundle) {
  $machine_name = '<?php echo $a->m; ?>';
  return _entityspice_entity_add_page($machine_name, $bundle);
}

// ____________________________________________________________________________

/**
 * Implements hook_entity_info().
 */
function <?php echo $a->m; ?>_entity_info() {
  $machine_name = '<?php print $a->m; ?>';
  return _entityspice_entity_info($machine_name);
}

function <?php echo $a->s; ?>_entity_name($entity) {
  // WARN always return check plained.
  return '?';
}

function <?php echo $a->s; ?>_entity_id($entity) {
  throw new RuntimeException('not supported');
}
