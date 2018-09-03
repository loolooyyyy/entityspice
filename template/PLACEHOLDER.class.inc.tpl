<?php print $a->php->value; ?>

/**
 * @file
 * <?php print $a->m; ?> entity classes.
 */

/**
 * The mighty entity class, for the entity itself.
 */
class <?php echo $a->uc; ?> extends Entity {
  const _entity_type = <?php print $a->m; ?>;

<?php if($a->has_bundle->value): ?>
  public $<?php print $a->bundle_key_name->value; ?> = NULL;
<?php endif; ?>

  // TODO custom prop

  public function __construct(array $values = []) {
    parent::__construct($values, $this::_entity_type);
  }

  final protected function defaultUri() {
    $base = "<?php echo $a->parent_userland_path->value; ?>";
    return ['path' => $base . '/' . $this->identifier()];
  }

}

// ___________________________________________________________________________

/**
 * The controller class for entities contains methods for the entity CRUD
 * operations.
 */
class <?php echo $a->uc; ?>EntityController extends EntityDefaultMetadataController {
  const _entity_type = '<?php print $a->m; ?>';

  public function __construct() {
    parent::__construct($this::_entity_type);
  }

<?php if($a->has_can_delete_hook->value): ?>
  /**
   * Deletes multiple entities by ID.
   *
   * adds can_delete hook support.
   *
   * @param array $entity_ids
   * @param \DatabaseTransaction|null $transaction
   *
   * @throws \Exception
   */
  public function delete($entity_ids, DatabaseTransaction $transaction = NULL) {
    if (empty($entity_ids)) {
      return;
    }
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
<?php endif; ?>

}

// ___________________________________________________________________________

/**
 * Entity Views Controller class.
 */
class <?php echo $a->uc; ?>ViewsController extends EntityDefaultViewsController {
  const _entity_type = '<?php print $a->m; ?>';

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
class <?php echo $a->uc ?>MetadataController extends EntityDefaultMetadataController {
  const _entity_type = '<?php print $a->m ?>';

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
