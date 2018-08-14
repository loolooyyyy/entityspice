<?php

/**
 * @file
 * Defines the inline entity form controller for entities.
 */

/**
 * Copied from RedHen.
 */
abstract class EntitySpiceInlineEntityFormController extends EntityInlineEntityFormController {

  /**
   * Overrides EntityInlineEntityFormController::labels().
   */
  public function labels() {
    // TODO remove?
    //    return [
    //      'singular' => t($this->entityType),
    //      'plural' => t($this->entityType . 's'),
    //    ];
    return parent::labels();
  }

  /**
   * Overrides EntityInlineEntityFormController::entityForm().
   */
  public function entityForm($f, &$fs) {
    $f = parent::entityForm($f, $fs);
    $entity = $f['#entity'];
    $info = _entityspice_get_info($this->entityType);

    // TODO?
    // Do some prep work on the entities, similarly to node_form().
    if ($info['has_title']) {
      $key = $info['title key'];
      $label = $info['title label'];
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

  /**
   * Overrides EntityInlineEntityFormController::entityFormSubmit().
   */
  public function entityFormSubmit(&$f, &$fs) {
    parent::entityFormSubmit($f, $fs);

    $entity = &$f['#entity'];
    $info = _entityspice_get_info($this->entityType);

    // $contact->first_name = $contact->name['first_name'];
    // $contact->last_name = $contact->name['last_name'];

    // Set the entity's author uid.
    if ($info['has_uid']) {
      $entity->uid = $GLOBALS['user']->uid;
    }

    // Save the entity.
    $entity = entity_save($this->entityType, $entity);
  }

}
