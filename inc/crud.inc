<?php

// _____________________________________________________________________ BUNDLE

/**
 * Deletes a bundle from the db.
 *
 * @param entity $bundle
 *   The entity representing the bundle.
 *
 * @return \entity
 */
function _entityspice_bundle_delete($etype, $bundle) {
  $bundle->delete();
  menu_rebuild();
  return $bundle;
}

/**
 * Load a entity bundle by string.
 *
 * @param string $bundle
 *   The machine-readable name of an entity bundle to load.
 *
 * @return array.
 *   An entity bundle array or FALSE if $bundle does not exist.
 */
function _entityspice_bundle_load($etype, $name) {
  return _entityspice_entity_get_bundles($etype, $name);
}

/**
 * Saves a model type to the db.
 */
function _entityspice_bundle_save($bundle) {
  $bundle->save();
  menu_rebuild();
  return $bundle;
}

// _____________________________________________________________________ ENTITY

/**
 * Create a new entity object.
 *
 * @param array $values
 *   Associative array of values. At least include ['type' => $type].
 *
 * @return \Entity created entity.
 */
function _entityspice_entity_create($etype, array $values = []) {
  /** @noinspection PhpUndefinedMethodInspection */
  return entity_get_controller($etype)->create($values);
}

/**
 * Loads an entity by ID.
 */
function _entityspice_entity_load($etype, $id) {
  if (empty($id)) {
    return NULL;
  }
  $entities = _entityspice_entity_load_multiple($etype, [$id]);
  return $entities ? reset($entities) : FALSE;
}

/**
 * Loads multiple entities by ID or based on a set of matching conditions.
 *
 * @param string $etype entity type
 * @param array $ids
 * @param array $conditions An array of conditions on the {entity_type} table
 *   in the form  'field' => $value.
 * @param bool $reset Whether to reset the internal entity loading cache.
 *
 * @return array An array of entity objects indexed by entity_id.
 *
 * @see entity_load()
 */
function _entityspice_entity_load_multiple($etype, array $ids = [], array $conditions = [], $reset = FALSE) {
  if (empty($ids) && empty($conditions)) {
    return [];
  }
  return entity_load($etype, $ids, $conditions, $reset);
}

/**
 * Deletes an entity by ID.
 *
 * @param string $etype entity type.
 * @param int $ids id of the entity to delete.
 *
 * @return bool TRUE on success, FALSE otherwise.
 */
function _entityspice_entity_delete($etype, $id) {
  return _entityspice_entity_delete_multiple($etype, [$id]);
}

/**
 * Deletes multiple entities by ID.
 *
 * @param string $etype entity type.
 * @param array $ids An array of entity IDs to delete.
 *
 * @return bool TRUE on success, FALSE otherwise.
 */
function _entityspice_entity_delete_multiple($etype, array $ids) {
  /** @noinspection PhpUndefinedMethodInspection */
  return entity_get_controller($etype)->delete($ids);
}

/**
 * Saves an entity.
 *
 * @param \Entity $entity The full entity object to save.
 *
 * @return \Entity The saved entity object.
 */
function _entityspice_entity_save($etype, $entity) {
  /** @noinspection PhpUndefinedMethodInspection */
  $entity->save();
  return $entity;
}

/**
 * Display an entity.
 *
 * @param string $etype entity type.
 * @param \Entity $entity
 * @param string $view_mode
 * @param boolean $not_found return drupal_not_found() if $entity is not valid.
 *
 * @return array render array.
 */
function _entityspice_entity_view($etype, $entity, $view_mode = 'full', $not_found = TRUE) {
  if ($entity = _entityspice_get_entity($etype, $entity)) {
    $view = $entity->view($view_mode);
  }
  elseif ($not_found) {
    drupal_not_found();
    return NULL;
  }
  return $view;
}

