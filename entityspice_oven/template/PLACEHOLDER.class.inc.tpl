<?php print $a['php'] ?>

/**
 * @file
 * <?php print $a['machine_name']; ?> entity classes.
 */

/**
 * The class used for entities.
 */
class <?php echo $a['machine_camel'] ?> extends EntitySpiceEntity {
  const _entity_type = <?php print $a['type_const'] ?>;

<?php if($a['has_title']): ?>
  public $title = NULL;
<?php endif; ?>

<?php if($a['has_bundle']): ?>
  public $<?php print $a['bundle_name'] ?> = NULL;
<?php endif; ?>

<?php if($a['has_uid']): ?>
  /**
   * Id of user who owns the entity.
   */
  public $<?php print $a['uid_name'] ?> = NULL;
<?php endif; ?>


<?php if($a['has_created']): ?>
  /**
   * The date entity was created on (UNIX timestamp).
   */
  public $<?php print $a['created_name'] ?> = NULL;
<?php endif; ?>

<?php if($a['has_updated']): ?>
  /**
   * The date entity was updated on (UNIX timestamp).
   */
  public $<?php print $a['updated_name'] ?> = NULL;
<?php endif; ?>

  public function __construct(array $values = []) {
    parent::__construct($values, $this::_entity_type);
  }

  // @codingStandardsIgnoreStart
  // @TODO ADD user properties.
  // @codingStandardsIgnoreEnd

<?php if($a['has_uid']): ?>
  /**
   * Set UID of user who owns this entity.
   */
  public function setOwner($<?php print $a['uid_name'] ?>) {
    $this-><?php print $a['uid_name'] ?> = $<?php print $a['uid_name'] ?>;
  }
<?php endif; ?>
}

// ___________________________________________________________________________

/**
 * The controller class for entities contains methods for the entity CRUD
 * operations.
 */
class <?php echo $a['machine_camel'] ?>EntityController extends EntitySpiceEntityController {
  const _entity_type = <?php print $a['type_const'] ?>;

  public function __construct() {
    parent::__construct($this::_entity_type);
  }


}

// ___________________________________________________________________________

/**
 * Entity Views Controller class.
 */
class <?php echo $a['machine_camel'] ?>ViewsController extends EntityDefaultViewsController {
  const _entity_type = <?php print $a['type_const'] ?>;

  public function __construct() {
    parent::__construct($this::_entity_type);
  }

  /**
   * Add extra fields to views_data().
   */
  public function views_data() {
    $data = parent::views_data();

    // @TODO ADD user views data

    return $data;
  }
}

// ___________________________________________________________________________

/**
 * Controls metadata for entities.
 */
class <?php echo $a['machine_camel'] ?>MetadataController extends EntitySpiceMetadataController {
  const _entity_type = <?php print $a['type_const'] ?>;

  public function __construct() {
    parent::__construct($this::_entity_type);
  }

  /**
   * Overrides entityPropertyInfo().
   */
  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

    // @TODO ADD user properties.

    return $info;
  }
}
