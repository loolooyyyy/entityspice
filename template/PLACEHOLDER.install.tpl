<?php echo $a->php; ?>

// TODO varchar
/**
 * Implements hook_schema().
 */
function <?php echo $a->m; ?>_schema() {
  $fields = [];

  $machine_name = '<?php echo $a->m; ?>';
  $base_table = '<?php echo $a->base_table; ?>';
  $id_key = '<?php echo $a->id_key; ?>';

<?php if($a->has_bundle->value): ?>
  $bundle_key_name = '<?php echo $a->bundle_key_name; ?>';
  $bundle_name_key = '<?php echo $a->bundle_name_key; ?>';
  $bundle_table = '<?php echo $a->bundle_table; ?>';
<?php endif; ?>
<?php if($a->has_revision->value): ?>
  $revision_key_name = '<?php echo $a->revision_key_name; ?>';
  $revision_table = '<?php echo $a->revision_table; ?>';
<?php endif; ?>

  $schema[$base_table] = [
    'description' => "the base table for $machine_name entity type",
    'primary key' => [$id_key],
  ];

  // TODO custom prop
  $schema[$base_table]['fields'][$id_key] = [
    'description' => 'primary key',
    'type' => 'serial',
    'unsigned' => TRUE,
    'not null' => TRUE,
  ];

<?php if($a->has_bundle->value): ?>
  $schema[$base_table]['fields'][$bundle_key_name] = [
    'description' => 'the bundle of entity',
    'type' => 'text',
    'size' => 'small',
    'not NULL' => TRUE,
  ];
<?php endif; ?>
<?php if($a->has_revision->value): ?>
  $schema[$revision_table]['fields'][$revision_key_name] = [
    'description' => 'revision of entity',
    'type' => 'serial',
    'unsigned' => TRUE,
    'not null' => TRUE,
  ];
<?php endif; ?>

<?php if($a->has_bundle->value): ?>
  $schema[$bundle_table] = [
    'description' => "stores information about all defined $machine_name bundles",
    'fields' => [
      $bundle_name_key => [
        'description' => 'machine readable name of entity bundle',
        'type' => 'varchar',
        'not NULL' => TRUE,
      ],
      'label' => [
        'description' => 'human readable name of bundle',
        'type' => 'varchar',
        'not NULL' => TRUE,
        'default' => '',
      ],
      'weight' => [
        'type' => 'int',
        'not NULL' => TRUE,
        'default' => 0,
        'size' => 'tiny',
        'description' => 'weight of bundle in relation to others',
      ],
      'locked' => [
        'description' => 'boolean flag indicating whether the administrator may delete the bundle',
        'type' => 'int',
        'not NULL' => TRUE,
        'default' => 0,
        'size' => 'tiny',
      ],
      'data' => [
        'type' => 'text',
        'not NULL' => FALSE,
        'size' => 'varchar',
        'serialize' => TRUE,
        'description' => 'serialized array of additional data related to the bundle',
        'merge' => TRUE,
      ],
      'status' => [
        'type' => 'int',
        'not NULL' => TRUE,
        'default' => 1,
        'size' => 'tiny',
        'description' => 'exportable status of the bundle',
      ],
      'module' => [
        'description' => 'name of the providing module if the entity has been defined in code',
        'type' => 'varchar',
        'not NULL' => FALSE,
      ],
    ],
    'primary key' => [$bundle_name_key],
    'unique keys' => [$bundle_name_key => [$bundle_name_key]],
  ];

  // TODO id type
  $schema[$bundle_table]['fields'][$bundle_name_key] = [
    'type' => 'varchar',
    'not NULL' => TRUE,
    'description' => 'unique bundle ID',
  ];
<?php endif; ?>

  return $schema;
}

/**
 * Implements hook_install().
 *
 * Create default bundle and default configuration.
 */
function <?php echo $a->m; ?>_install() {
 //if(!drupal_get_schema('<?php echo $a->m; ?>_bundle')) {
 //  return;
 //}
 //else {
 //  $bundle = entity_create('<?php echo $a->m; ?>_bundle', []);
 //  $bundle->locked = TRUE;
 //  $bundle->label = '<?php echo $a->m ?>';
 //  $bundle->name = '<?php echo $a->m; ?>';
 //  $bundle->save();
 //}
}
