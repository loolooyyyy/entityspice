<?php

/**
 * Implements hook_entity_info_alter().
 *
 * We are adding the info about the entity bundles via a hook to avoid a
 * recursion issue as loading the model types requires the entity info as well.
 */
function _entityspice_entity_info_alter($etype, &$entity_info) {
  // @TODO make sure $etype is actually the path in $base_path.
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
 *
 * @return array|mixed
 * @throws \EntitySpiceOperationNotSupported
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
 *
 * @param string $etype entity type
 *
 * @return mixed
 */
function _entityspice_entity_info($etype) {
  // TODO
  // entity label mistaken for title property here


  $info = _entityspice_get_info($etype);

  $ret_info[$etype] = [
    'label' => $info->label(),
    'controller class' => $info->controllerClass(),
    'metadata controller class' => $info->metadataControllerClass(),
    'entity class' => $info->entityClass(),
    'views controller class' => $info->viewsControllerClass(),
    'base table' => $info->baseTable(),
    'fieldable' => $info->isFieldable(),
    'entity keys' => [
      'id' => $info->idKey(),
      'bundle' => $info->bundleKeyName(),
      'label' => $info->label(),
    ],
    'bundle keys' => [
      'bundle' => $info->bundleKeyName(),
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
    'module' => $info->module(),
    'label callback' => 'entity_class_label',
    'access callback' => $info->accessCallback(),
  ];
  if ($info->hasBundle()) {
    $ret_info[$info->bundleMachineName()] = [
      'label' => $info->label() . ' Bundle',
      'entity class' => $info->bundleClass(),
      'controller class' => $info->bundleControllerClass(),
      'base table' => $info->bundleTable(),
      'fieldable' => $info->isBundleFieldable(),
      'bundle of' => $etype,
      'exportable' => TRUE,
      'token type' => $info->bundleMachineName(),
      'entity keys' => [
        'id' => $info->bundleIdKey(),
        'name' => 'name',
        'label' => 'label',
      ],
      'access callback' => $info->bundleAccessCallback(),
      'module' => $info->module(),
      'admin ui' => [
        'path' => $info->parentAdminPath(),
        //         'file' => $file,
        //         'file path' => $file_path,
        'controller class' => $info->bundleUIControllerClass(),
      ],
    ];
  }
  if ($info->hasRevision()) {
    $ret_info[$etype]['revision table'] = $info->revisionTable();
    $ret_info[$etype]['entity keys']['revision'] = $info->ridKey();
  }
  if ($info->hasTitle()) {
    $ret_info[$etype]['entity keys']['label'] = $info->label();
  }

  return $ret_info;
}

// ____________________________________________________________________ DATABASE

/**
 * Executes a filter query against the provided attributes.
 *
 * Also used to filter entity list pages.
 *
 * TODO too damn complex.
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
 * @throws \Exception
 */
function _entityspice_filter_query($entity_type, $header, $bundle = '', $properties = [], $fields = [], $items_page = 10) {
  $query = new EntityFieldQuery();

  $query
    ->entityCondition('entity_type', $entity_type, '=')
    ->tablesort($header)
    ->pager($items_page);

  if (!empty($bundle)) {
    $query->entityCondition('bundle', $bundle, '=');
  }

  // Add property filters.
  foreach ($properties as $key => $value) {
    $property_info = entity_get_all_property_info($entity_type);
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
 * TODO my awesome idea for custom fields.
 *
 * @param string $etype
 *   Machine name of entity type, Will be name of base table.
 *
 * @param \EntitySpiceEntityConfig $info
 *   When calling hook_schema in a modules module.install, the module's
 *   hook_entityspice_info does not exist yet, so this must be manually passed.
 *
 * @return array entity schema
 */
function _entityspice_entity_schema($etype, EntitySpiceEntityConfig $info) {
  $revision_table = $info->revisionTable();
  $base_table = $info->baseTable();

  $schema[$base_table] = [
    'description' => "the base table for $etype entity",
    'primary key' => [$info->idKey()],
  ];

  $schema[$base_table]['fields'] = [
    $info->idKey() => [
      'description' => 'primary key of the entity',
      'type' => 'serial',
      'unsigned' => TRUE,
      'not null' => TRUE,
    ],
    $info->uidKey() => [
      'description' => 'owner of the entity',
      'type' => 'int',
      'unsigned' => TRUE,
      'not null' => TRUE,
    ],
    'created' => [
      'description' => 'the Unix timestamp of the entity creation time',
      'type' => 'int',
      'not null' => TRUE,
      'default' => 0,
    ],
    'updated' => [
      'description' => 'The Unix timestamp when the entity was updated',
      'type' => 'int',
      'not null' => TRUE,
      'default' => 0,
    ],
    'title' => [
      'description' => 'Title',
      'type' => 'varchar',
      'length' => '255',
      'not null' => TRUE,
      'default' => '',
    ],
  ];

  if (!$info->hasUid()) {
    unset($schema[$base_table]['fields'][$info->uidKey()]);
  }
  if (!$info->hasCreated()) {
    unset($schema[$base_table]['fields']['created']);
  }
  if (!$info->hasUpdated()) {
    unset($schema[$base_table]['fields']['updated']);
  }
  if (!$info->hasTitle()) {
    unset($schema[$base_table]['fields']['title']);
  }
  if ($info->hasRevision()) {
    $schema[$base_table]['fields'][$info->ridKey()] = [
      'description' => 'the primary identifier for version',
      'type' => 'int',
      'unsigned' => TRUE,
      'not null' => FALSE,
    ];

    $schema[$revision_table] = $schema[$base_table];
    $schema[$revision_table]['description'] = "stores information about each saved version of a $etype";
    $schema[$revision_table]['primary key'] = [$info->ridKey()];
    $schema[$revision_table]['fields'][$info->idKey()]['type'] = 'int';
    $schema[$revision_table]['fields'][$info->ridKey()]['type'] = 'serial';
    $schema[$revision_table]['fields']['log'] = [
      'description' => 'the log entry explaining the changes in this version',
      'type' => 'text',
      'not null' => FALSE,
      'size' => 'big',
    ];

    if (!$info->hasUpdated()) {
      unset($schema[$base_table]['fields']['updated']);
    }
    if (!$info->hasRevisionLog()) {
      unset($schema[$revision_table]['fields']['log']);
    }
  }
  if ($info->hasBundle()) {
    $schema[$base_table]['fields'][$info->bundleKeyName()] = [
      'description' => 'the bundle type',
      'type' => 'text',
      'size' => 'small',
      'not NULL' => TRUE,
    ];
    $schema[$revision_table]['fields'][$info->bundleKeyName()] = [
      'description' => 'the bundle type',
      'type' => 'text',
      'size' => 'small',
      'not NULL' => TRUE,
    ];

    $schema[$info->bundleTable()] = [
      'description' => "stores information about all defined $etype bundles",
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
      'primary key' => [$info->bundleIdKey()],
      'unique keys' => ['name' => ['name']],
    ];

    if ($info->bundleIdKey() !== 'name') {
      $schema[$info->bundleTable()]['fields'][$info->bundleIdKey()] = [
        'type' => 'serial',
        'not NULL' => TRUE,
        'description' => 'unique bundle ID',
      ];
    }
    else {
      $schema[$info->bundleTable()]['fields'][$info->bundleIdKey()]['type'] = 'serial';
    }
  }

  return $schema;
}

// _____________________________________________________________________ ACCESS

/**
 * Checks entity access for various operations.
 *
 * @param string $etype entity type.
 * @param string $op The operation being performed. Such as view, update, ...
 * @param string/null $bundle
 * @param object/null $entity
 * @param object/int/null $user user with uid, or the uid, or null for current
 *   user.
 *
 * @return bool
 */
function _entityspice_entity_access($etype, $op, $bundle = NULL, $entity = NULL, $user = NULL) {
  if ($user !== NULL && is_object($user)) {
    $account = $user->uid;
  }
  elseif ($user !== NULL && ctype_digit($user)) {
    $account = $user;
  }
  elseif ($user === NULL) {
    $account = $GLOBALS['user'];
  }
  else {
    throw new RuntimeException();
  }

  $info = _entityspice_get_info($etype);
  if (!in_array($op, $info->accessControlledActions())) {
    return FALSE;
  }

  $self = $entity !== NULL && $info->hasUid() && isset($entity->{$info->uidKey()})
    ? ($entity->{$info->uidKey()} == $account->uid) : FALSE;

  if ($self && $bundle !== NULL && user_access("own - $etype - $bundle - $op", $account)) {
    return TRUE;
  }
  if ($self && user_access("own - $etype - $op", $account)) {
    return TRUE;
  }
  if ($bundle !== NULL && user_access("$etype - $bundle - $op")) {
    return TRUE;
  }
  if (user_access("$etype - $op")) {
    return TRUE;
  }

  return FALSE;
}

