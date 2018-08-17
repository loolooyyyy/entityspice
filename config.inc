<?php

// TODO create delegate.

/**
 * Configuration keys of an entity supported by entity spice.
 *
 * None of methods declared in this class or implementation should take a key,
 * and nothing other than simple values indication support for a future should
 * be returned. An implementation must be a simple immutable value class with
 * no logic. Else, it is a design error, a disaster.
 */
interface EntitySpiceEntityConfig {

  function module();

  function machineName();

  function humanName();

  function camelNameLowerFirst();

  function camelNameUpperFirst();


  function idKey();

  function titleKey();

  function ridKey();

  function uidKey();

  // ___________________________________________________________________ BUNDLE

  function hasBundle();

  function bundleIdKey();

  function isBundleFieldable();

  function bundleMachineName();

  function bundleKeyName();

  function bundleTable();

  function bundleAccessCallback();

  function bundleAdminFile();

  function hasBundleAdminFile();

  function bundleFormSubmitDelete();

  function bundleSubmitDelete();

  function bundleExistsCallback();

  function bundleUIControllerClass();

  function bundleClass();

  function bundleControllerClass();

  function bundleSubmitRedirect();

  function bundleDeleteRedirect();


  // _________________________________________________________________ REVISION


  function hasRevision();

  function revisionTable();

  function hasRevisionLog();

  function revisionKeyName();

  // ___________________________________________________________________ ENTITY

  function baseTable();

  function titleLabel();

  function hasTitle();

  function hasUid();

  function hasUpdated();

  function hasCreated();

  function isFieldable();

  function accessCallback();

  function controllerClass();

  function metadataControllerClass();

  function viewsControllerClass();

  function entityClass();

  function uriBase();

  function uriCallback();

  function hasCanDeleteHook();

  // ______________________________________________________________ ENTITY FORM

  function hasFormsFile();

  function addForm();

  function editForm();

  function deleteForm();

  function formsFile();

  function entityFormSubmitCallback();

  function entityDeleteFormSubmitCallback();

  function entityFormValidationCallback();


  function parentAdminPath();

  function parentUserlandPath();

  function entityDeleteRedirect();

  function accessControlledActions();


  // __________________________________________________________________________

  function hasInlineEntityFormSupport();

  function inlineInlineEntityForm();


  /**
   * NOT implemented.
   */
  function bundleIsEntity();

}

/**
 * Default implementation of \EntitySpiceEntityConfiguration with sane values.
 *
 * This implementation does NOT call any of subclass methods. Do NOT expect
 * behaviour of a method to change, overriding some other method.
 */
class DefaultEntitySpiceEntityConfig implements EntitySpiceEntityConfig {

  private $machine_name;

  private $label_name;

  private $camel_name_upper_first;

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

  function camelNameLowerFirst() {
    return lcfirst($this->camel_name_upper_first);
  }

  function camelNameUpperFirst() {
    return $this->camel_name_upper_first;
  }

  function humanName() {
    return $this->label_name;
  }

  function bundleMachineName() {
    return $this->machine_name . '_bundle';
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

  function formsFile() {
    // Final slash is needed.
    return [
      'directory' => '/',
      'file' => $this->machine_name . ".forms",
      'suffix' => 'inc',
    ];
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


  function hasFormsFile() {
    return TRUE;
  }

  function bundleAdminFile() {
    return $this->machine_name . 'admin.inc';
  }

  function hasBundleAdminFile() {
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

  function entityFormSubmitCallback() {
    return $this->machine_name . '_form_submit';
  }

  function entityDeleteFormSubmitCallback() {
    return $this->machine_name . '_delete_form_submit';
  }

  function entityFormValidationCallback() {
    return $this->machine_name . '_form_validate';
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

  /*
   *
  // This variable needed in hook_menu so that ES will know what will be the
  // position of arg in each path. It is determined by how many words between
  // slashes (es/mytype/% : 2) appear before the arg.
  $d['parent user path arg no'] =
  count(explode('/', $d['parent userland path']));
   * @return array
   */


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

  function bundleUIControllerClass() {
    return $this->camel_name_upper_first . 'BundleUIController';
  }


  function bundleClass() {
    return $this->camel_name_upper_first . 'Bundle';
  }

  function bundleControllerClass() {
    return $this->camel_name_upper_first . 'BundleController';
  }

}