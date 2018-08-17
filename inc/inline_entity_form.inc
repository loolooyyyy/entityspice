<?php

/**
 * @file
 * Defines the inline entity form controller for entities.
 */

abstract class EntitySpiceInlineEntityFormController extends EntityInlineEntityFormController {

  public function labels() {
    // TODO remove?
    //    return [
    //      'singular' => t($this->entityType),
    //      'plural' => t($this->entityType . 's'),
    //    ];
    return parent::labels();
  }

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
