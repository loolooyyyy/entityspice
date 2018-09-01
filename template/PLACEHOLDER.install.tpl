<?php print $a['php'] ?>

/**
 * Implements hook_schema().
 */
function <?php echo $a['machine_name'] ?>_schema() {
  $fields = [];

  $revision_table = NULL; // $info->revisionTable();
  $base_table = NULL; // $info->baseTable();
  $machine_name = NULL;
  $id_key = NULL;

  $bundle_key_name = NULL;
  $bundle_id_key = NULL;
  $bundle_table = NULL;
  $has_bundle = NULL;

  $schema[$base_table] = [
    'description' => "the base table for $machine_name entity",
    'primary key' => [$id_key],
  ];

  // TODO custom prop
  $schema[$base_table]['fields'] = [
    $id_key => [
      'description' => 'primary key of the entity',
      'type' => 'serial',
      'unsigned' => TRUE,
      'not null' => TRUE,
    ],
  ];

  if ($has_bundle) {
    $schema[$base_table]['fields'][$bundle_key_name] = [
      'description' => 'the bundle type',
      'type' => 'text',
      'size' => 'small',
      'not NULL' => TRUE,
    ];
    $schema[$revision_table]['fields'][$bundle_key_name] = [
      'description' => 'the bundle type',
      'type' => 'text',
      'size' => 'small',
      'not NULL' => TRUE,
    ];

    $schema[$bundle_table] = [
      'description' => "stores information about all defined $machine_name bundles",
      'fields' => [
        'name' => [
          'description' => 'machine readable name of entity bundle',
          'type' => 'tiny',
          'not NULL' => TRUE,
        ],
        'label' => [
          'description' => 'human readable name of bundle',
          'type' => 'tiny',
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
        'export_status' => [
          'type' => 'int',
          'not NULL' => TRUE,
          'default' => 1,
          'size' => 'tiny',
          'description' => 'exportable status of the bundle',
        ],
        'module' => [
          'description' => 'name of the providing module if the entity has been defined in code',
          'type' => 'tiny',
          'not NULL' => FALSE,
        ],
      ],
      'primary key' => [$bundle_id_key],
      'unique keys' => ['name' => ['name']],
    ];

    // TODO id type
    $schema[$bundle_table]['fields'][$bundle_id_key] = [
      'type' => 'string',
      'not NULL' => TRUE,
      'description' => 'unique bundle ID',
    ];
  }

  return $schema;
}

/**
 * Implements hook_install().
 *
 * Create default bundle and default configuration.
 */
function <?php echo $a['machine_name'] ?>_install() {
 //if(!drupal_get_schema('<?php echo $a['machine_name'] ?>_bundle')) {
 //  return;
 //}
 //else {
 //  $bundle = entity_create('<?php echo $a['machine_name'] ?>_bundle', []);
 //  $bundle->locked = TRUE;
 //  $bundle->label = '<?php echo $a['label'] ?>';
 //  $bundle->name = '<?php echo $a['machine_name'] ?>';
 //  $bundle->save();
 //}
}
