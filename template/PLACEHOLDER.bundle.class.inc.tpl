<?php print $a['php']; ?>

/**
 * @file
 * <?php print $a['machine_name']; ?>'s bundle classes.
 *
// TODO custom fields.
// TODO deny delete.
// TODO Entity default label.
// TODO custom fields on build content.
 */

/**
 * The class used for bundle entities
 */
class <?php echo $a->camelNameUcFirst(); ?>Bundle extends Entity {

  /**
   * String: Machine name of bundle entity.
   */
  public $name;

  /**
   * String: Human readable name (label) of bundle entity.
   */
  public $label;

<?php if($a->bundleHasLock()): ?>
  /**
   * Boolean: Whether deleting this bundle is allowed or not.
   */
  public $locked;
<?php endif; ?>

  public function __construct(array $values = []) {
    parent::__construct($values, '<?php print $a->bundleMachineName(); ?>');
  }
}

/**
 * The controller class for entity bundles contains methods for CRUD
 * operations. The load method is inherited from the default controller.
 */
class <?php print $a->camelNameUcFirst(); ?>Controller extends EntityAPIControllerExportable {

  /**
   * @see parent
   */
  final public function delete($ids, DatabaseTransaction $transaction = NULL) {
    <?php if($a->hasLock());>
    foreach($ids as $id) {
      if(<?php echo $a->sMachineName(); ?>_bundle_has_entity($id)) {
        throw new RuntimeException('entities of this type exist, thus it is locked: [ ' . $id . ']');
      }
    }
    <?php endif;>
    parent::delete($ids, $transaction);
    menu_rebuild();
  }

  public function __construct() {
    parent::__construct('<?php print $a->bundleMachineName(); ?>');
  }
}


/**
 * Entity Bundle UI controller.
 */
class <?php print $a->camelNameUcFirst(); ?>BundleUIController extends EntityDefaultUIController {
  public function __construct($entity_info) {
    parent::__construct('<?php print $a->bundleMachineName(); ?>', $entity_info);
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
function <?php echo $a->sMachineName() ?>_bundle_create(array $values = []) {
  $bundle_machine_name = '<?php echo $a->bundleMachineName(); ?>';
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
function <?php echo $a->sMachineName() ?>_bundle_load($name) {
  $bundle_machine_name = '<?php echo $a->bundleMachineName(); ?>';

  $bundles = entity_load_multiple_by_name($bundle_machine_name, [$name]);
  foreach ($bundles as $bundle) {
    return $bundle;
  }

  throw new <?php echo $a->exceptionClass(); >('no such bundle: ' . $name);
}

/**
* Deletes a bundle.
*
* @param entity $bundle the bundle to delete (bundle object)
*/
function <?php echo $a->sMachineName(); ?>_bundle_delete($bundle) {
  $bundle->delete();
}

/**
* Saves a bundle.
*
* @param entity $bundle the bundle to save (bundle object)
*/
function <?php echo $a->sMachineName(); ?>_bundle_save($bundle) {
  $bundle->save();
}

// ____________________________________________________________________________________________________________________

/**
 * Gets an array of all bundles, keyed by the name.
 */
function <?php echo $a->sMachineName(); ?>_get_bundles() {
  $bundle_machine_name = '<?php echo $a->bundleMachineName(); ?>';
  return entity_load_multiple_by_name($bundle_machine_name);
}

/**
 * Generate a list of bundle names of a given entity type.
 */
function <?php echo $a->sMachineName(); ?>_get_bundles_names() {
  $bundles = <?php echo $a->sMachineName(); ?>_get_bundles();
  return array_keys($bundles);
}

/**
 * TODO does it need check plain?
 */
function <?php echo $a->sMachineName(); ?>_get_bundles_options_list($check_plain = TRUE) {
  $bundles = <?php echo $a->sMachineName(); ?>_get_bundles();
  $ret = [];
  foreach ($bundles as $bundle => $info) {
    $ret[$bundle] = $check_plain ? check_plain($info->label) : $info->label;
  }
  return $ret;
}

/**
* Check if entities of this bundle exist.
*/
function <?php echo $a->sMachineName(); ?>_bundle_has_entity($bundle_machine_name) {
  $bundle_machine_name = '<?php echo $a->bundleMachineName(); ?>';
  throw new RuntimeException('must check if any entity of this bundle exists, but it is not implemented yet!');
}
