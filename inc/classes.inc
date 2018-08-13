<?php

// ______________________________________________________________________ BUNDLE

/**
 * The controller class for bundle entities.
 */
abstract class EntitySpiceBundleController extends EntityAPIControllerExportable {

  /**
   * Delete one or more entity bundles.
   *
   * Overrides EntityAPIControllerExportable::delete().
   *
   * @param array $ids
   *   Array of entity bundle IDs.
   * @param DatabaseTransaction $transaction
   *   Optionally a DatabaseTransaction object to use. Allows overrides to pass
   *   in their transaction object.
   *
   * @throws \Exception
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    // TODO do not allow delete if entity exists for this bundle (configurable).

    $transaction = isset($transaction) ? $transaction : db_transaction();
    try {
      $query = new EntityFieldQuery();
      $result = $query
        ->entityCondition('entity_type', $this->entityType)
        ->entityCondition('bundle', $ids, 'IN')
        ->execute();

      if ($result) {
        $ids = array_keys($result[$this->entityType]);
        _entityspice_entity_delete_multiple($this->entityType, $ids);
      }

      parent::delete($ids, $transaction);
      menu_rebuild();
    } catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception($this->entityType, $e);
      throw $e;
    }
  }

}

/**
 * The class used for bundle entities.
 */
abstract class EntitySpiceBundleEntity extends Entity {

}

/**
 * Entity type UI controller.
 */
abstract class EntitySpiceBundleEntityUIController extends EntityDefaultUIController {

  /**
   * Overrides parent::operationForm to set a more meaningful message on delete.
   *
   * Delegates to parent on other operations.
   *
   * @FIXME when bundle does not have label.
   *
   * @param $form
   * @param $form_state
   * @param $entity
   * @param $op
   *
   * @return mixed
   */
  public function operationForm($form, &$form_state, $entity, $op) {
    switch ($op) {
      case 'delete':
        $label = entity_label($this->entityType, $entity);
        $targ = ['%entity' => $this->entityInfo['label'], '%label' => $label];
        $confirm_question = t('Are you sure you want to delete the %entity %label? ', $targ);
        $desc = t('This action will also delete <strong>all</strong> ' .
          'entities of type %label and cannot be undone.', $targ);
        return confirm_form($form, $confirm_question, $this->path, $desc);

      default:
        return parent::operationForm($form, $form_state, $entity, $op);
    }
  }

}

// _____________________________________________________________________ CONTENT

/**
 * The controller class for entities. contains methods for the entity CRUD
 * operations. The load method is inherited from the default controller.
 */
abstract class EntitySpiceEntityController extends EntityAPIController {

  /**
   * Saves an entity.
   *
   * @param $entity
   *   The full entity object to save.
   *
   * @param \DatabaseTransaction|null $transaction
   *
   * @return entity
   *   The saved entity object.
   * @throws \Exception
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    $entity->updated = REQUEST_TIME;
    if (isset($entity->is_new) && $entity->is_new) {
      // New entity, set created property.
      $entity->created = REQUEST_TIME;
    }
    else {
      // Set revision flags. By default, save a new revision and set it as
      // default.
      if (!isset($entity->is_new_revision)) {
        $entity->is_new_revision = TRUE;
      }
      if (!isset($entity->default_revision)) {
        $entity->default_revision = TRUE;
      }
    }

    parent::save($entity, $transaction);

    module_invoke_all($this->entityType . '_save', $entity);
    return $entity;
  }

  /**
   * Deletes multiple entities by ID.
   *
   * Checks to see if each entity can be removed or not and if not, ignore it.
   * In case none of them are allowed to be deleted returns FALSE.
   *
   * @param array $entity_ids
   *   An array of entity IDs to delete.
   *
   * @return bool
   *   TRUE on success, FALSE otherwise.
   * @throws \Exception
   */
  public function delete($entity_ids, DatabaseTransaction $transaction = NULL) {
    if (!empty($entity_ids)) {
      $entities = $this->load($entity_ids, []);

      // Ensure the entities can actually be deleted.
      $hook0 = $this->entityType . '_can_delete';
      $hook1 = $this->entityType . '_predelete';
      foreach ((array) $entities as $entity_id => $entity) {
        if (in_array(FALSE, module_invoke_all($hook0, $entity, $this->entityType))) {
          unset($entities[$entity_id]);
        }
        else {
          module_invoke_all($hook1, $entity, $this->entityType);
        }
      }

      if (empty($entities)) {
        // If none of the specified entities can be deleted, return FALSE.
        return FALSE;
      }
      else {
        // Otherwise, update the list of entity IDs to those which are deletable.
        $entity_ids = array_keys($entities);
      }

      parent::delete($entity_ids, $transaction);
    }

    return TRUE;
  }

}

/**
 * The class used for entities.
 */
abstract class EntitySpiceEntity extends Entity {

  protected function defaultUri() {
    $uri = _entityspice_get_info($this->entityType)['default uri base'];
    return ['path' => $uri . '/' . $this->identifier()];
  }

  protected function defaultLabel() {
    return $this->wrapper()->title->value();
  }

  /**
   * Override buildContent() to add entity properties.
   */
  public function buildContent($view_mode = 'full', $langcode = NULL, array $content = []) {
    if (_entityspice_get_info($this->entityType)['has_title']) {
      $content['title'] = [
        '#theme' => 'entityspice_entity_property_field',
        '#label' => t('Title'),
        '#items' => ['#markup' => check_plain($this->wrapper()->title->value())],
        '#classes' => 'field field-label-inline clearfix',
      ];
    }

    return entity_get_controller($this->entityType)
      ->buildContent($this, $view_mode, $langcode, $content);
  }

}

/**
 *  Controls metadata for entity.
 *
 *  entityspice utilizes custom property attributes to determine if a property
 *  should be available as a filter on the listing page. These attributes are
 *  filter: TRUE to add as a filter.
 *  filter_operator: EFQ supported operators. Defaults to = or IN depending on
 *  value submitted
 *  field_type: textfield, select, etc.
 *  options list: callback that returns the options for this field.
 */
abstract class EntitySpiceMetadataController extends EntityDefaultMetadataController {

  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

    $admin_permission = 'administer ' . $this->type;

    $spicy_info = _entityspice_get_info($this->type);

    // TODO custom properties at generation time

    if ($spicy_info['has bundle']) {
      $key = $spicy_info['bundle key name'];
      $properties[$key] = [
        'label' => t('Bundle'),
        'description' => t('The bundle of the entity.'),
        'type' => 'token',
        'setter callback' => 'entity_property_verbatim_set',
        'setter permission' => $admin_permission,
        'options list' => $this->type . '_bundle_options_list',
        'required' => TRUE,
        'schema field' => $spicy_info['bundle key name'],
      ];
    }

    // @FIXME make title field name customizable.
    if ($spicy_info['has_title']) {
      $properties['title'] = [
        'label' => t('Title'),
        'type' => 'text',
        'filter' => TRUE,
        'description' => t('Title of the entity.'),
        'setter callback' => 'entity_property_verbatim_set',
        'entity views field' => TRUE,
      ];
    }

    if ($spicy_info['has_created']) {
      $properties['created'] = [
        'entity views field' => TRUE,
        'label' => t('Created'),
        'description' => t('The date the entity was created.'),
        'type' => 'date',
        'schema field' => 'created',
        'setter callback' => 'entity_property_verbatim_set',
        'setter permission' => $admin_permission,
      ];
    }

    if ($spicy_info['has_updated']) {
      $properties['updated'] = [
        'entity views field' => TRUE,
        'label' => t('Updated'),
        'description' => t('The date the entity was updated.'),
        'type' => 'date',
        'schema field' => 'updated',
        'setter callback' => 'entity_property_verbatim_set',
        'setter permission' => $admin_permission,
      ];
    }

    if ($spicy_info['has_uid']) {
      $properties['uid'] = [
        'label' => t('Owner'),
        'type' => 'user',
        'description' => t('The owner of the entity.'),
        'schema field' => 'uid',
        'setter permission' => $admin_permission,
        'entity views field' => TRUE,
      ];
    }

    return $info;
  }

}
