<?php

// TODO custom fields.
// TODO deny delete.
// TODO Entity default label.
// TODO custom fields on build content.

// ______________________________________________________________________ BUNDLE

/**
 * The controller class for bundle entities.
 */
abstract class EntitySpiceBundleController extends EntityAPIControllerExportable {

  /**
   * Delete one or more entity bundles.
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
    parent::delete($ids, $transaction);
    menu_rebuild();
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

}

// _____________________________________________________________________ CONTENT

/**
 * The controller class for entities.
 *
 * contains methods for the entity CRUD operations. The load method is
 * inherited from the default controller.
 */
abstract class EntitySpiceEntityController extends EntityAPIController {

  /**
   * Deletes multiple entities by ID.
   *
   * Checks to see if each entity can be removed or not and if not, ignore it.
   * In case none of them are allowed to be deleted returns FALSE.
   *
   * @param array $entity_ids
   *   An array of entity IDs to delete.
   *
   * @param \DatabaseTransaction|null $transaction
   *
   * @throws \Exception
   */
  public function delete($entity_ids, DatabaseTransaction $transaction = NULL) {
    if (empty($entity_ids)) {
      return;
    }
    if (_entityspice_get_info($this->entityType)->hasCanDeleteHook()) {
      $hook = $this->entityType . '_can_delete';
      $ids = [];
      foreach ($this->load($entity_ids) as $entity_id => $entity) {
        if (!in_array(FALSE, module_invoke_all($hook, $entity, $this->entityType))) {
          $ids[] = $entity_id;
        }
      }
      if (!empty($ids)) {
        parent::delete($ids, $transaction);
      }
    }
    else {
      parent::delete($entity_ids, $transaction);
    }
  }

}

/**
 * The mighty entity class, for the entity itself.
 */
abstract class EntitySpiceEntity extends Entity {

  protected function defaultUri() {
    $uri = _entityspice_get_info($this->entityType)->uriBase();
    return ['path' => $uri . '/' . $this->identifier()];
  }

  /**
   * Override buildContent() to add entity properties.
   *
   * @param string $view_mode
   * @param null $langcode
   * @param array $content
   *
   * @return mixed
   */
  public function buildContent($view_mode = 'full', $langcode = NULL, array $content = []) {
    if (_entityspice_get_info($this->entityType)->hasTitle()) {
      $content['title'] = [
        '#theme' => 'entityspice_entity_property_field',
        // TODO
        '#label' => t('Title'),
        // TODO
        '#items' => ['#markup' => check_plain($this->wrapper()->title->value())],
        // TODO
        '#classes' => 'field field-label-inline clearfix',
      ];
    }

    return entity_get_controller($this->entityType)
      ->buildContent($this, $view_mode, $langcode, $content);
  }

  //  protected function defaultLabel() {
  //    return $this->wrapper()->title->value();
  //  }

}

/**
 *  Controls metadata for entity.
 *
 * TODO what?
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

    $spicy_info = _entityspice_get_info($this->type);

    // TODO custom properties at generation time
    // FIXME make title field name customizable.

    $properties[$spicy_info->bundleKeyName()] = [
      'label' => t('Bundle'),
      'description' => t('The bundle of the entity.'),
      'type' => 'token',
      'setter callback' => 'entity_property_verbatim_set',
      'setter permission' => 'administer ' . $this->type,
      'options list' => $this->type . '_bundle_options_list',
      'required' => TRUE,
      'schema field' => $spicy_info->bundleKeyName(),
    ];
    $properties['title'] = [
      'label' => t('Title'),
      'type' => 'text',
      'filter' => TRUE,
      'description' => t('Title of the entity.'),
      'setter callback' => 'entity_property_verbatim_set',
      'entity views field' => TRUE,
    ];
    $properties['created'] = [
      'entity views field' => TRUE,
      'label' => t('Created'),
      'description' => t('The date the entity was created.'),
      'type' => 'date',
      'schema field' => 'created',
      'setter callback' => 'entity_property_verbatim_set',
      'setter permission' => 'administer ' . $this->type,
    ];
    $properties['updated'] = [
      'entity views field' => TRUE,
      'label' => t('Updated'),
      'description' => t('The date the entity was updated.'),
      'type' => 'date',
      'schema field' => 'updated',
      'setter callback' => 'entity_property_verbatim_set',
      'setter permission' => 'administer ' . $this->type,
    ];
    $properties['uid'] = [
      'label' => t('Owner'),
      'type' => 'user',
      'description' => t('The owner of the entity.'),
      'schema field' => 'uid',
      'setter permission' => 'administer ' . $this->type,
      'entity views field' => TRUE,
    ];

    if (!$spicy_info->hasBundle()) {
      unset($properties[$spicy_info->bundleKeyName()]);
    }
    if (!$spicy_info->hasTitle()) {
      unset($properties['title']);
    }
    if (!$spicy_info->hasCreated()) {
      unset($properties['created']);
    }
    if (!$spicy_info->hasUpdated()) {
      unset($properties['updated']);
    }
    if (!$spicy_info->hasUid()) {
      unset($properties['uid']);
    }

    return $info;
  }

}
