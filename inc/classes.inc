<?php

// TODO
abstract class EntitySpiceEntity extends Entity {


  /**
   * Override buildContent() to add entity properties.
   *
   * @param string $view_mode
   * @param null $langcode
   * @param array $content
   *
   * @return mixed
   */
  final public function buildContent($view_mode = 'full', $langcode = NULL, array $content = []) {
    // TODO all custom fields.
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

// TODO
abstract class EntitySpiceMetadataController extends EntityDefaultMetadataController {

  final public function entityPropertyInfo() {
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

// TODO
abstract class EntitySpiceInlineEntityFormController extends EntityInlineEntityFormController {

  public function entityForm($f, &$fs) {
    $f = parent::entityForm($f, $fs);
    $entity = $f['#entity'];
    $info = _entityspice_get_info($this->entityType);
    // TODO?
    // Do some prep work on the entities, similarly to node_form().
    if ($info->hasTitle()) {
      $key = $info->titleKey();
      $label = $info->titleLabel();
      if (!isset($entity->{$key})) {
        $entity->{$key} = '';
      }
      $f[$key] = [
        '#type' => 'textfield',
        '#title' => t($label),
        '#default_value' => $entity->{$key},
        '#maxlength' => 255,
        '#required' => TRUE,
        '#weight' => -6,
      ];
    }
    return $f;
  }

  public function entityFormSubmit(&$f, &$fs) {
    parent::entityFormSubmit($f, $fs);
    $entity = &$f['#entity'];
    $info = _entityspice_get_info($this->entityType);
    if ($info->hasUid()) {
      $entity->{$info->uidKey()} = $GLOBALS['user']->uid;
    }
    $entity = entity_save($this->entityType, $entity);
  }

}
