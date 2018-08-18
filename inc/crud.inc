<?php

// _____________________________________________________________________ BUNDLE

/**
 * Create a new bundle object.
 *
 * TODO remove
 *
 * @param string $etype type of bundle to create.
 * @param array $values
 *   Associative array of values. At least include ['type' => $type].
 *
 * @return \Entity created bundle.
 */
function _entityspice_bundle_create($etype, array $values = []) {
  return _entityspice_entity_create($etype, $values);
}

/**
 * Load a entity bundle by string.
 *
 * TODO remove
 *
 * @param string $etype entity type the requested bundle belongs to.
 * @param string $name The machine-readable name of a bundle to load.
 *
 * @return array.
 *   An entity bundle array or FALSE if $bundle does not exist.
 */
function _entityspice_bundle_load($etype, $name) {
  return _entityspice_entity_get_bundle($etype, $name);
}

/**
 * Deletes a bundle of an entity type.
 *
 * TODO remove
 *
 * @param entity $bundle entity object representing the bundle.
 */
function _entityspice_bundle_delete($bundle) {
  $bundle->delete();
}

/**
 * Saves a bundle to the db.
 *
 * TODO remove
 *
 * @param entity $bundle the bundle to delete.
 */
function _entityspice_bundle_save($bundle) {
  $bundle->save();
}

// _____________________________________________________________________ ENTITY

/**
 * Create a new entity object.
 *
 * TODO remove
 *
 * @param string $etype type of entity to create.
 * @param array $values
 *   Associative array of values. At least include ['type' => $type].
 *
 * @return \Entity created entity.
 */
function _entityspice_entity_create($etype, array $values = []) {
  return entity_get_controller($etype)->create($values);
}

/**
 * Loads an entity by ID.
 *
 * TODO remove
 *
 * @param string $etype type of entity being loaded.
 * @param mixed $id integer id (or any other type) uniquely identifying an
 *   entity.
 *
 * @return \Entity the loaded entity or NULL if nothing was loaded.
 */
function _entityspice_entity_load($etype, $id) {
  $load = entity_load($etype, [$id]);
  return isset($load[$id]) ? $load[$id] : NULL;
}

/**
 * @see entity_load()
 *
 * TODO remove
 *
 * @param $etype
 * @param bool $ids
 * @param array $conditions
 * @param bool $reset
 *
 * @return mixed
 */
function _entityspice_entity_load_multiple($etype, $ids = FALSE, array $conditions = [], $reset = FALSE) {
  return entity_load($etype, $ids, $conditions, $reset);
}

/**
 * Deletes an entity by ID.
 *
 * TODO remove
 *
 * @param string $etype entity type.
 * @param mixed $id
 *
 * @return bool TRUE on success, FALSE otherwise.
 */
function _entityspice_entity_delete($etype, $id) {
  return _entityspice_entity_delete_multiple($etype, [$id]);
}

/**
 * Deletes multiple entities by ID.
 *
 * TODO remove
 *
 * @param string $etype entity type.
 * @param array $ids An array of entity IDs to delete.
 *
 * @return bool TRUE on success, FALSE otherwise.
 */
function _entityspice_entity_delete_multiple($etype, array $ids) {
  return entity_get_controller($etype)->delete($ids);
}

/**
 * Saves an entity.
 *
 * TODO remove
 *
 * @param object $entity the entity object to save.
 *
 * @return object
 */
function _entityspice_entity_save($entity) {
  $entity->save();
  return $entity;
}

/**
 * Save multiple entities.
 *
 * TODO remove
 *
 * @param array $entities list of entity objects to save.
 *
 * @return array list of saved entity objects.
 */
function _entityspice_entity_save_multiple(array $entities) {
  foreach ($entities as $entity) {
    $entity->save();
  }
  return $entities;
}

/**
 * Display an entity.
 *
 * @TODO remove
 *
 * @param string $etype entity type.
 * @param \Entity $entity
 * @param string $view_mode
 * @param boolean $drupal_not_found_on_not_found return drupal_not_found() if $entity is not valid.
 *
 * @return array render array.
 */
function _entityspice_entity_view($etype, $entity, $view_mode = 'full', $drupal_not_found_on_not_found = TRUE) {
  if ($entity = _entityspice_get_entity($etype, $entity)) {
    return $entity->view($view_mode);
  }
  elseif ($drupal_not_found_on_not_found) {
    drupal_not_found();
  }
  return NULL;
}
