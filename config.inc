<?php

/**
 * This implementation does NOT call any of subclass methods. Do NOT expect
 * behaviour of a method to change, overriding some other method.
 *
 * None of methods declared in this class or implementation should take a key,
 * and nothing other than simple values indication support for a future should
 * be returned. It must be a simple immutable value class with no logic. Else,
 * it is a design error, a disaster.
 */
class DefaultEntitySpiceEntityConfig {

  private $machine_name;

  private $label_name;

  private $camel_name_upper_first;

  private $safe;

  /**
   * @param $machine_name string machine name of entity this config belongs to.
   */
  public function __construct($machine_name) {
    if (empty($machine_name)
      || !is_string($machine_name)
      || !ctype_alnum($machine_name)
      || ctype_digit($machine_name[0])
      || $machine_name[0] === '_') {
      throw new RuntimeException("bad machine name: " .
        (is_string($machine_name) ? $machine_name : 'NON_STRING_GIVEN'));
    }

    $this->safe = '_';
    $this->machine_name = $machine_name;
    $this->label_name = ucwords(str_replace('_', ' ', $machine_name));
    $this->camel_name_upper_first = str_replace(' ', '', $this->label_name);
  }


  function module() {
    return $this->machine_name;
  }

  function machineName() {
    return $this->machine_name;
  }

  function machineNameConst() {
    return '_ETYPE__' . strtoupper($this->machine_name);
  }

  function safe() {
    return $this->safe;
  }


  function camelNameLcFirst() {
    return lcfirst($this->camel_name_upper_first);
  }

  function camelNameUcFirst() {
    return $this->camel_name_upper_first;
  }

  function humanName() {
    return $this->label_name;
  }

  function bundleMachineName() {
    return $this->machine_name . '_bundle';
  }

  function bundleEntityClass() {
    return $this->camel_name_upper_first . 'Bundle';
  }

  function bundleControllerClass() {
    return $this->camel_name_upper_first . 'BundleController';
  }

  function bundleIdKey() {
    return "bid";
  }

  function bundleKeyName() {
    return "bid";
  }

  function idKey() {
    return "eid";
  }

  function ridKey() {
    return "rid";
  }

  function uidKey() {
    return "uid";
  }

  function titleKey() {
    return "title";
  }

  function titleLabel() {
    return "Title";
  }

  function baseTable() {
    return $this->machine_name;
  }

  function revisionTable() {
    return $this->machine_name . '_revision';
  }

  function bundleTable() {
    return $this->machine_name . '_bundle';
  }

  function addForm() {
    return $this->machine_name . '_form';
  }

  function editForm() {
    return $this->machine_name . '_form';
  }

  function deleteForm() {
    return $this->machine_name . '_delete_form';
  }

  function uriBase() {
    return str_replace('_', '-', $this->machine_name);
  }


  function hasTitle() {
    return TRUE;
  }

  function hasUid() {
    return TRUE;
  }

  function hasUpdated() {
    return TRUE;
  }

  function hasCreated() {
    return TRUE;
  }

  function hasRevision() {
    return FALSE;
  }

  function hasRevisionLog() {
    return TRUE;
  }

  function hasBundle() {
    return TRUE;
  }


  function isFieldable() {
    return TRUE;
  }

  function isBundleFieldable() {
    return TRUE;
  }


  function accessCallback() {
    return $this->machine_name . '_access';
  }

  function bundleAccessCallback() {
    return $this->machine_name . '_bundle_access';
  }

  function bundleFormSubmitDelete() {
    // TODO
    return '_entityspice_entity_bundle_form_submit_delete';
  }

  function bundleSubmitDelete() {
    // TODO
    return '_entityspice_entity_bundle_form_submit_delete';
  }

  function bundleExistsCallback() {
    return $this->machine_name . '_get_bundles';
  }


  function parentAdminPath() {
    return 'admin/structure/' . $this->machine_name;
  }

  function parentUserlandPath() {
    return $this->machine_name;
  }

  function bundleSubmitRedirect() {
    return 'admin/structure/' . $this->machine_name;
  }

  function bundleDeleteRedirect() {
    return 'admin/structure/' . $this->machine_name;
  }

  function entityDeleteRedirect() {
    return '/';
  }


  function hasInlineEntityFormSupport() {
    return TRUE;
  }

  function inlineInlineEntityForm() {
    return $this->camel_name_upper_first . 'InlineEntityFormController';
  }

  function accessControlledActions() {
    return [
      'administer',
      'create',
      'delete',
      'view',
      'archive',
    ];
  }

  function bundleIsEntity() {

  }


  function controllerClass() {
    return $this->camel_name_upper_first . 'EntityController';
  }

  function metadataControllerClass() {
    return $this->camel_name_upper_first . 'MetadataController';
  }

  function viewsControllerClass() {
    return $this->camel_name_upper_first . 'ViewsController';
  }

  function entityClass() {
    return $this->camel_name_upper_first;
  }

  function exceptionClass() {
    return $this->camel_name_upper_first . 'Exception';
  }

  function bundleUIControllerClass() {
    return $this->camel_name_upper_first . 'BundleUIController';
  }


  function bundleClass() {
    return $this->camel_name_upper_first . 'Bundle';
  }

  function revisionKeyName() {
    return 'rid';
  }

  function uriCallback() {
    throw new RuntimeException("uriCallback");
  }

  function hasCanDeleteHook() {
    return FALSE;
  }

  function bundleLockOnExists() {
    return TRUE;
  }

  function inlineInlineEntityFormClass() {
    return $this->camel_name_upper_first . 'InlineEntityFormController';
  }

  function develSupport() {
    return TRUE;
  }
}
