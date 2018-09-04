<?php print $a->php; ?>

/**
 * @file
 * <?php print $a->m; ?>'s bundle classes.
 *
// TODO custom fields.
// TODO deny delete.
// TODO Entity default label.
// TODO custom fields on build content.
 */

/**
 * The class used for bundle entities
 */
class <?php echo $a->uc; ?>Bundle extends Entity {

  const _entity_type = '<?php print $a->bundle_machine_name->value; ?>';

  /**
   * String: Machine name of bundle entity.
   */
  public $name;

  /**
   * String: Human readable name (label) of bundle entity.
   */
  public $label;

<?php if($a->bundle_has_lock->value): ?>
  /**
   * Boolean: Whether deleting this bundle is allowed or not.
   */
  public $locked;
<?php endif; ?>

  public function __construct(array $values = []) {
    parent::__construct($values, self::_entity_type);
  }
}

/**
 * The controller class for entity bundles contains methods for CRUD
 * operations. The load method is inherited from the default controller.
 */
class <?php print $a->uc; ?>Controller extends EntityAPIControllerExportable {

  const _entity_type = '<?php print $a->bundle_machine_name->value; ?>';

  /**
   * @see parent
   */
  final public function delete($ids, DatabaseTransaction $transaction = NULL) {
    <?php if($a->bundle_has_lock->value): ?>
    foreach($ids as $id) {
      if(<?php echo $a->s; ?>_bundle_has_entity($id)) {
        throw new RuntimeException('entities of this type exist, it is locked and can not be deleted: [ ' . $id . ']');
      }
    }
    <?php endif; ?>
    parent::delete($ids, $transaction);
    menu_rebuild();
  }

  public function __construct() {
    parent::__construct(self::_entity_type);
  }
}


/**
 * Entity Bundle UI controller.
 */
class <?php print $a->uc; ?>BundleUIController extends EntityDefaultUIController {
  const _entity_type = '<?php print $a->bundle_machine_name->value; ?>';

  public function __construct($entity_info) {
    parent::__construct(self::_entity_type, $entity_info);
  }
}

/**
* Create a new bundle object - DOES NOT SAVE IT.
*
* @param array $values
*   Associative array of values. At least include ['type' => $type].
*
* @return \Entity created bundle.
*/
function <?php echo $a->s ?>_bundle_create(array $values = []) {
  $bundle_machine_name = '<?php echo $a->bundle_machine_name->value; ?>';
  return entity_get_controller($bundle_machine_name)->create($values);
}

/**
* Load a bundle entity by it's machine name.
*
* @param string $name The machine-readable name of a bundle to load.
*
* @return array.
*   An bundle entity array or FALSE if $name does not exist.
*/
function <?php echo $a->s ?>_bundle_load($name) {
  $bundle_machine_name = '<?php echo $a->bundle_machine_name->value; ?>';

  $bundles = entity_load_multiple_by_name($bundle_machine_name, [$name]);
  foreach ($bundles as $bundle) {
    return $bundle;
  }

  throw new <?php echo $a->uc; ?>Exception('no such bundle: ' . $name);
}

/**
* Deletes a bundle.
*
* @param entity $bundle the bundle to delete (bundle object)
*/
function <?php echo $a->s; ?>_bundle_delete($bundle) {
  $bundle->delete();
}

/**
* Saves a bundle.
*
* @param entity $bundle the bundle to save (bundle object)
*/
function <?php echo $a->s; ?>_bundle_save($bundle) {
  $bundle->save();
}

// ____________________________________________________________________________________________________________________

/**
 * Gets an array of all bundles, keyed by the name.
 */
function <?php echo $a->s; ?>_get_bundles() {
  $bundle_machine_name = '<?php echo $a->bundle_machine_name->value; ?>';
  return entity_load_multiple_by_name($bundle_machine_name);
}

/**
 * Generate a list of bundle names of a given entity type.
 */
function <?php echo $a->s; ?>_get_bundles_names() {
  $bundles = <?php echo $a->s; ?>_get_bundles();
  return array_keys($bundles);
}

/**
 * TODO does it need check plain?
 */
function <?php echo $a->s; ?>_get_bundles_options_list($check_plain = TRUE) {
  $bundles = <?php echo $a->s; ?>_get_bundles();
  $ret = [];
  foreach ($bundles as $bundle => $info) {
    $ret[$bundle] = $check_plain ? check_plain($info->label) : $info->label;
  }
  return $ret;
}

/**
* Check if entities of this bundle exist.
*/
function <?php echo $a->s; ?>_bundle_has_entity($name) {
  $machine_name = '<?php echo $a->m ?>';
  $bundle_machine_name = '<?php echo $a->bundle_machine_name->value; ?>';
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', $machine_name);
  // TODO
  $query->entityCondition('bundle', $name); // IS THIS RIGHT?
  return $query->count()->execute() > 0;
}
