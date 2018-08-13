<?php

/**
 * Implements hook_entity_info_alter().
 *
 * We are adding the info about the entity bundles via a hook to avoid a
 * recursion issue as loading the model types requires the entity info as well.
 */
function _entityspice_entity_info_alter($etype, &$entity_info) {
  // @TODO make sure ${etypes}s is actually the path in $base_path.
  // @todo: Testing to ensure the schema exists; needed because running gui
  // install profile was hitting this BEFORE the schema was installed.
  if (drupal_get_schema($etype)) {
    $i = _entityspice_get_info($etype);
    $base_path = $i['_entityspice_entity_info_alter']['path'];
    $real_path = $i['_entityspice_entity_info_alter']['real path base'];

    foreach (_entityspice_entity_get_bundles($etype) as $bundle => $info) {
      $entity_info[$etype]['bundles'][$bundle] = [
        'label' => $info->label,
        'admin' => [
          'path' => $base_path,
          'real path' => $real_path . '/' . "$bundle",
          'bundle argument' => $i['_entityspice_entity_info_alter']['bundle argument'],
          'access arguments' => ["administer $etype bundles"],
        ],
      ];
    }
  }
}

/**
 * Title callback, returns title of entity.
 */
function _entityspice_entity_title($etype, $entity, $check_plain = TRUE) {
  if (!_entityspice_get_info($etype)['has_title']) {
    throw new EntitySpiceOperationNotSupported("entity type [$etype] does not have a title property");
  }
  if ($entity = _entityspice_get_entity($etype, $entity)) {
    $label = _entityspice_get_entity($etype, $entity)->label();
    return $check_plain ? check_plain($label) : $entity->label();
  }
}

/**
 * Implements hook_field_extra_field_info().
 *
 * Needs bundle support on entity type.
 */
function _entityspice_field_extra_field_info($etype, array $form = [], array $display = []) {
  $extra = [];

  if (_entityspice_get_info($etype)['has_title']) {
    foreach (_entityspice_entity_get_bundles($etype) as $bundle => $info) {
      $extra[$etype][$bundle] = [
        'form' => [
          'title' => [
              'label' => t('Title'),
              'description' => t('Entity title'),
              'weight' => 0,
            ] + $form,
        ],
        'display' => [
            'title' => [
              'label' => t('Title'),
              'weight' => 0,
            ],
          ] + $display,
      ];
    }
  }

  return $extra;
}

/**
 * Helper function to load entity if it's ID is passed.
 *
 * @param string $etype
 *   machine name of entity type.
 * @param int/object $entity
 *   Entity object itself or id of it.
 *
 * @return entity
 *   Loaded entity.
 */
function _entityspice_get_entity($etype, $entity) {
  if (!is_object($entity)) {
    $entity = _entityspice_entity_load($etype, $entity);
  }

  return is_object($entity) ? $entity : NULL;
}

/**
 * Gets an array of all entity types, keyed by the name.
 *
 * @param string $name
 *   If set, the bundle with the given name is returned.
 */
function _entityspice_entity_get_bundles($etype, $name = NULL) {
  if (!_entityspice_get_info($etype)) {
    throw new EntitySpiceOperationNotSupported("entity type [$etype] does not implement a bundle system.");
  }

  $bundle_name = _entityspice_get_info($etype)['bundle_machine_name'];
  $bundles =
    entity_load_multiple_by_name($bundle_name, $name ? [$name] : FALSE);

  return $name ? reset($bundles) : $bundles;
}

/**
 * Generate a list of bundle names of a given entity type.
 *
 * Default arguments MUST be in a way so that calling the function without any
 * arguments generates an apropirate list of names for form API.
 *
 * @param string $etype
 *   Entity type.
 * @param string $with_label
 *   Return associative array instead, Which has bundle name as key and label
 *   as value. Will be good for form API.
 * @param string $check_plain
 *   Whether to pass bundle label through checkp_plain or not.
 *
 * @return array bundle names.
 */
function _entityspice_entity_get_bundles_names($etype, $with_label = TRUE, $check_plain = TRUE) {
  if (!_entityspice_get_info($etype)) {
    throw new EntitySpiceOperationNotSupported("entity type [$etype] does not implement a bundle system.");
  }

  $bundles = _entityspice_entity_get_bundles($etype);

  if ($with_label) {
    $ret = [];
    foreach ($bundles as $bundle => $info) {
      $ret[$bundle] = $check_plain ? check_plain($info->label) : $info->label;
    }
  }
  else {
    $ret = array_keys($bundles);
  }

  return $ret;
}

/**
 * Implements hook_entity_info().
 */
function _entityspice_entity_info($etype) {
  $info = _entityspice_get_info($etype);

  $path = $info['parent admin path'];
  $concat_camel = $info['concat_camel'];
  $label = $info['label'];
  $type_idkey = $info['bundle_id_key'];
  $bundle = $info['bundle_machine_name'];
  $module = $info['module'];

  $ret_info[$etype] = [
    'label' => $label,
    'controller class' => $concat_camel . 'EntityController',
    'metadata controller class' => $concat_camel . 'MetadataController',
    'entity class' => $concat_camel,
    'views controller class' => $concat_camel . 'ViewsController',
    'base table' => $info['base table'],
    'fieldable' => $info['is fieldable'],
    'entity keys' => [
      'id' => $info['idkey'],
      'bundle' => $info['bundle key name'],
      'label' => 'title',
    ],
    'bundle keys' => [
      'bundle' => 'name',
    ],
    'bundles' => [],
    'view modes' => [
      'full' => [
        'label' => t('Full'),
        'custom settings' => FALSE,
      ],
    ],
    'uri callback' => 'entity_class_uri',
    'token type' => $etype,
    'module' => $module,
    'label callback' => 'entity_class_label',
    'access callback' => $info['access callback'],
  ];
  if ($info['has_bundle']) {
    $ret_info[$bundle] = [
      'label' => t("$label Bundle"),
      'entity class' => $concat_camel . 'Bundle',
      'controller class' => $concat_camel . 'BundleController',
      'base table' => $info['bundle table'],
      'fieldable' => $info['is bundle fieldable'],
      'bundle of' => $etype,
      'exportable' => TRUE,
      'token type' => $bundle,
      'entity keys' => [
        'id' => $type_idkey,
        'name' => 'name',
        'label' => 'label',
      ],
      'access callback' => $info['bundle access callback'],
      'module' => $module,
      'admin ui' => [
        'path' => $path,
        //         'file' => $file,
        //         'file path' => $file_path,
        'controller class' => $concat_camel . 'BundleUIController',
      ],
    ];
  }
  if ($info['has_revision']) {
    $ret_info[$etype]['revision table'] = $info['revision table'];
    $ret_info[$etype]['entity keys']['revision'] = $info['ridkey'];
  }
  if ($info['has_title']) {
    // TODO customize
    $ret_info[$etype]['entity keys']['label'] = 'title';
  }

  return $ret_info;
}

// ____________________________________________________________________ DATABASE

/**
 * Executes a filter query against the provided attributes.
 *
 * Also used to filter entity list pages.
 *
 * @param string $entity_type
 *   Entity type to filter by.
 * @param array $header
 *   Header array for sorting. Should match the table header used in output.
 * @param string $bundle
 *   Entity bundle.
 * @param array $properties
 *   Associative array of key/value pairs.
 * @param array $fields
 *   Entity fields to filter by.
 * @param int $items_page
 *   Results per page.
 *
 * @return array
 *   Executed query object.
 */
function _entityspice_filter_query($entity_type, $header, $bundle = '', $properties = [], $fields = [], $items_page = 10) {
  $has_revision = _entityspice_get_info($entity_type)['has_revision'];

  $query = new EntityFieldQuery();

  $query
    ->entityCondition('entity_type', $entity_type, '=')
    ->tablesort($header)
    ->pager($items_page);

  if (!empty($bundle) && $has_revision) {
    $query->entityCondition('bundle', $bundle, '=');
  }

  // Add property filters.
  foreach ($properties as $key => $value) {
    $property_info = entity_get_all_property_info($entity_type);
    if ($value != '') {
      // Processing for date ranges.
      if ($property_info[$key]['type'] == 'date') {
        if ($value['from'] != '') {
          $conditions = [
            strtotime($value['from']),
            // Add 24 hours for full day search.
            strtotime($value['to']) + 86400,
          ];
          $query->propertyCondition($key, $conditions, 'BETWEEN');
        }
      }
      else {
        // NULL operator will allow literal or array for $value.
        $operator = NULL;
        if (isset($property_info[$key]['filter_operator']) && !is_array($value)) {
          $operator = $property_info[$key]['filter_operator'];
        }
        $query->propertyCondition($key, $value, $operator);
      }
    }
  }
  // Add field filters.
  foreach ($fields as $key => $field) {
    $field_info = field_info_field($key);

    switch ($field_info['type']) {
      // Special handling for date fields, which have a two-value range.
      case 'date':
      case 'datestamp':
      case 'datetime':
        if ($field[LANGUAGE_NONE][0]['value'] != '' && $field[LANGUAGE_NONE][0]['value2'] != '') {
          $value = [
            $field[LANGUAGE_NONE][0]['value'],
            $field[LANGUAGE_NONE][0]['value2'],
          ];
          $query->fieldCondition($key, 'value', $value, 'BETWEEN', $key);
        }
        break;

      case 'list_boolean':
        foreach ($field[LANGUAGE_NONE][0] as $column => $value) {
          // Accommodate the skip option introduced in _entityspice_filter_form().
          if ($value != 'skip') {
            $query->fieldCondition($key, $column, (int) $value, '=', $key);
          }
        }
        break;

      default:
        // Accept only valid columns. Some field types add non-column data.
        $field[LANGUAGE_NONE][0] = array_intersect_key($field[LANGUAGE_NONE][0], $field_info['columns']);
        foreach ($field[LANGUAGE_NONE][0] as $column => $value) {
          // Only accept fields for which a value has been entered.
          if ($value != '') {
            $query->fieldCondition($key, $column, $value, 'CONTAINS', $key);
          }
        }
        break;
    }
  }

  return $query->execute();
}

/**
 * Populate database schema array for entity.
 *
 * @param string $etype
 *   Machine name of entity type, Will be name of base table.
 *
 * @param array $additional_fields
 * @param array $overrides
 *
 * @return mixed
 */
function _entityspice_entity_schema($etype, array $additional_fields = [], array $overrides = []) {
  $info = _entityspice_get_info_override($etype, $overrides);

  $idKey = $info['idkey'];
  $revision_key = $info['ridkey'];
  $revision_table = $info['revision table'];;
  $bundle_idkey = $info['bundle_id_key'];
  $bundle_table = $info['bundle table'];
  $bundle_key = $info['bundle key name'];
  $base_table = $info['base table'];

  $schema[$base_table] = [
    'description' => "The base table for $etype entity.",
    'primary key' => [$idKey],
  ];

  $schema[$base_table]['fields'] = $additional_fields + [
      $idKey => [
        'description' => 'Primary key of the entity.',
        'type' => 'serial',
        'unsigned' => TRUE,
        'not null' => TRUE,
      ],

      'uid' => [
        'description' => 'Owner of the entity.',
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
      ],


      'created' => [
        'description' => 'The Unix timestamp of the entity creation time.',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ],
    ];

  if ($info['has_title']) {
    $schema[$base_table]['fields']['title'] = [
      'description' => 'Title',
      'type' => 'varchar',
      'length' => '255',
      'not null' => TRUE,
      'default' => '',
    ];
  }

  if ($info['has_revision']) {
    $schema[$base_table]['fields']['updated'] = [
      'description' => 'The Unix timestamp when the entity was most recently saved.',
      'type' => 'int',
      'not null' => TRUE,
      'default' => 0,
    ];

    $schema[$base_table]['fields'][$revision_key] = [
      'description' => 'The primary identifier for version.',
      'type' => 'int',
      'unsigned' => TRUE,
      'not null' => FALSE,
    ];

    $schema[$revision_table] = $schema[$base_table];

    $schema[$revision_table]['description'] = "Stores information about each saved version of a $etype.";
    $schema[$revision_table]['primary key'] = [$revision_key];

    $schema[$revision_table]['fields'][$idKey]['type'] = 'int';
    $schema[$revision_table]['fields'][$revision_key]['type'] = 'serial';
    $schema[$revision_table]['fields']['log'] = [
      'description' => 'The log entry explaining the changes in this version.',
      'type' => 'text',
      'not null' => FALSE,
      'size' => 'big',
    ];
  }

  if ($info['has bundle']) {
    $schema[$base_table]['fields'][$bundle_key] = [
      'description' => 'The bundle type',
      'type' => 'text',
      'size' => 'medium',
      'not null' => TRUE,
    ];

    $schema[$revision_table]['fields'][$bundle_key] = [
      'description' => 'The bundle type',
      'type' => 'text',
      'size' => 'medium',
      'not null' => TRUE,
    ];

    $schema[$bundle_table] = [
      'description' => "Stores information about all defined $etype bundles.",
      'fields' => [
        $bundle_idkey => [
          'type' => 'serial',
          'not null' => TRUE,
          'description' => 'Primary Key: Unique bundle ID.',
        ],

        'name' => [
          'description' => 'The machine-readable name of entity bundle.',
          'type' => 'varchar',
          'length' => 32,
          'not null' => TRUE,
        ],

        'label' => [
          'description' => 'The human-readable name of this entity bundle.',
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
          'default' => '',
        ],

        'weight' => [
          'type' => 'int',
          'not null' => TRUE,
          'default' => 0,
          'size' => 'tiny',
          'description' => 'The weight of this bundle in relation to others.',
        ],

        'locked' => [
          'description' => 'A boolean indicating whether the administrator may delete this type.',
          'type' => 'int',
          'not null' => TRUE,
          'default' => 0,
          'size' => 'tiny',
        ],

        'data' => [
          'type' => 'text',
          'not null' => FALSE,
          'size' => 'big',
          'serialize' => TRUE,
          'description' => 'A serialized array of additional data related to this bundle.',
          'merge' => TRUE,
        ],

        'status' => [
          'type' => 'int',
          'not null' => TRUE,
          // Set the default to ENTITY_CUSTOM without using the constant as it is
          // not safe to use it at this point.
          'default' => 0x01,
          'size' => 'tiny',
          'description' => 'The exportable status of the entity.',
        ],

        'module' => [
          'description' => 'The name of the providing module if the entity has been defined in code.',
          'type' => 'varchar',
          'length' => 255,
          'not null' => FALSE,
        ],
      ],

      'primary key' => [$bundle_idkey],

      'unique keys' => ['name' => ['name']],
    ];
  }

  return $schema;
}

// _____________________________________________________________________ ACCESS

/**
 * Checks entity access for various operations.
 *
 * @param string $etype entity type.
 * @param string $op The operation being performed. One of 'view',
 *   'update', 'create', 'delete' or 'archive'.
 * @param entity $entity entity to check access for, or for the create
 *   operation, the entity bundle. If nothing is given access permissions for
 *   all entities of given type are returned.
 *
 * @return bool
 */
function _entityspice_entity_access($etype, $op, $entity = NULL, $user = NULL) {
  $account = $user ? $user : $GLOBALS['user'];
  $self = $entity && isset($entity->uid) ? ($entity->uid == $account->uid) : FALSE;
  $hasType = isset($entity) && is_string($entity);

  if (user_access("administer $etype", $account)) {
    return TRUE;
  }

  switch ($op) {
    case 'edit':
      return ($self && user_access("edit own $etype", $account))
        || user_access("manage $etype", $account);
    case 'view':
      return ($self && user_access("view own $etype", $account))
        || user_access("access $etype", $account);
    case 'create':
      return user_access("manage $etype", $account) ||
        ($hasType && user_access("create $entity $etype", $account));
    case 'delete':
      return user_access("manage $etype", $account);
    case 'archive':
      return user_access("manage $etype", $account);
  }

  return FALSE;
}

/**
 * Access callback for the entity API.
 */
function _entityspice_entity_bundle_access($etype, $op, $bundle = NULL, $account = NULL) {
  return user_access("administer $etype bundles", $account);
}

/**
 * Define permissions useful for entities.
 *
 * @param string $type
 *   Entity type (machine name).
 * @param string $label
 *   Entity label (human redable).
 *
 * @return array
 *   Array of permissions related to entities.
 */
function _entityspice_entity_permissions($type, $label) {
  $perms = [
    "administer $type bundles" => [
      'title' => t("Administer $label Bundles"),
      'description' => t("Manage $label bundles and their structure."),
    ],
    "administer $type" => [
      'title' => t("Administer $type"),
      'description' => t("Perform administration tasks for $label."),
    ],
    "manage $type" => [
      'title' => t("Manage $label"),
      'description' => t("Create, update or delete $label."),
    ],
    "access $type" => [
      'title' => t("Access $label"),
      'description' => t("View $label."),
    ],
    "view own $type" => [
      'title' => t("Access Own $label"),
      'description' => t("View own $label information."),
    ],
    "edit own $type" => [
      'title' => t("Edit Own $label"),
      'description' => t("Edit own $label information."),
    ],
  ];
  return $perms;
}
