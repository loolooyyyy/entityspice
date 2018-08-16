<?php print $a['php']; ?>

/**
 * @file
 * <?php print $a['machine_name']; ?>'s bundle entity classes.
 */

/**
 * The class used for bundle entities
 */
class <?php echo $a['machine_camel'] ?>Bundle extends EntitySpiceBundleEntity {
  const _entity_type = <?php print $a['type_const'] ?>;

  /**
   * String: Machine name of bundle entity.
   */
  public $name;

  /**
   * String: Human readable name (label) of bundle entity.
   */
  public $label;

  /**
   * Boolean: Whether deleting this bundle is allowed or not.
   */
  public $locked;

  public function __construct(array $values = []) {
    parent::__construct($values, $this::_entity_type);
  }
}

/**
 * The controller class for entity bundles contains methods for CRUD
 * operations. The load method is inherited from the default controller.
 */
class <?php print $a['machine_camel'] ?>BundleController extends EntitySpiceBundleController {
  const _entity_type = <?php print $a['type_const'] ?>;

  public function __construct() {
    parent::__construct($this::_entity_type);
  }
}


/**
 * Entity Bundle UI controller.
 */
class <?php print $a['machine_camel'] ?>BundleUIController extends EntitySpiceBundleEntityUIController {
  const _entity_type = <?php print $a['type_const'] ?>;

  public function __construct($entity_info) {
    parent::__construct($this::_entity_type, $entity_info);
  }
}
