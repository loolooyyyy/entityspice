<?php

// TODO
abstract class EntitySpiceEntity extends Entity {


  /**
   * Override buildContent() to add entity properties.
   *
   * @param string $view_mode
   * @param null $langcode
   * @param array $content
   *
   * @return mixed
   */
  final public function buildContent($view_mode = 'full', $langcode = NULL, array $content = []) {
    // TODO all custom fields.
    if (_entityspice_get_info($this->entityType)->hasTitle()) {
      $content['title'] = [
        '#theme' => 'entityspice_entity_property_field',
        // TODO
        '#label' => t('Title'),
        // TODO
        '#items' => ['#markup' => check_plain($this->wrapper()->title->value())],
        // TODO
        '#classes' => 'field field-label-inline clearfix',
      ];
    }

    return entity_get_controller($this->entityType)
      ->buildContent($this, $view_mode, $langcode, $content);
  }

  //  protected function defaultLabel() {
  //    return $this->wrapper()->title->value();
  //  }

}

// TODO
abstract class EntitySpiceMetadataController extends EntityDefaultMetadataController {

  final public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

    $spicy_info = _entityspice_get_info($this->type);

    // TODO custom properties at generation time
    // FIXME make title field name customizable.

    $properties[$spicy_info->bundleKeyName()] = [
      'label' => t('Bundle'),
      'description' => t('The bundle of the entity.'),
      'type' => 'token',
      'setter callback' => 'entity_property_verbatim_set',
      'setter permission' => 'administer ' . $this->type,
      'options list' => $this->type . '_bundle_options_list',
      'required' => TRUE,
      'schema field' => $spicy_info->bundleKeyName(),
    ];
    $properties['title'] = [
      'label' => t('Title'),
      'type' => 'text',
      'filter' => TRUE,
      'description' => t('Title of the entity.'),
      'setter callback' => 'entity_property_verbatim_set',
      'entity views field' => TRUE,
    ];
    $properties['created'] = [
      'entity views field' => TRUE,
      'label' => t('Created'),
      'description' => t('The date the entity was created.'),
      'type' => 'date',
      'schema field' => 'created',
      'setter callback' => 'entity_property_verbatim_set',
      'setter permission' => 'administer ' . $this->type,
    ];
    $properties['updated'] = [
      'entity views field' => TRUE,
      'label' => t('Updated'),
      'description' => t('The date the entity was updated.'),
      'type' => 'date',
      'schema field' => 'updated',
      'setter callback' => 'entity_property_verbatim_set',
      'setter permission' => 'administer ' . $this->type,
    ];
    $properties['uid'] = [
      'label' => t('Owner'),
      'type' => 'user',
      'description' => t('The owner of the entity.'),
      'schema field' => 'uid',
      'setter permission' => 'administer ' . $this->type,
      'entity views field' => TRUE,
    ];

    if (!$spicy_info->hasBundle()) {
      unset($properties[$spicy_info->bundleKeyName()]);
    }
    if (!$spicy_info->hasTitle()) {
      unset($properties['title']);
    }
    if (!$spicy_info->hasCreated()) {
      unset($properties['created']);
    }
    if (!$spicy_info->hasUpdated()) {
      unset($properties['updated']);
    }
    if (!$spicy_info->hasUid()) {
      unset($properties['uid']);
    }

    return $info;
  }

}

// TODO
abstract class EntitySpiceInlineEntityFormController extends EntityInlineEntityFormController {

  public function entityForm($f, &$fs) {
    $f = parent::entityForm($f, $fs);
    $entity = $f['#entity'];
    $info = _entityspice_get_info($this->entityType);
    // TODO?
    // Do some prep work on the entities, similarly to node_form().
    if ($info->hasTitle()) {
      $key = $info->titleKey();
      $label = $info->titleLabel();
      if (!isset($entity->{$key})) {
        $entity->{$key} = '';
      }
      $f[$key] = [
        '#type' => 'textfield',
        '#title' => t($label),
        '#default_value' => $entity->{$key},
        '#maxlength' => 255,
        '#required' => TRUE,
        '#weight' => -6,
      ];
    }
    return $f;
  }

  public function entityFormSubmit(&$f, &$fs) {
    parent::entityFormSubmit($f, $fs);
    $entity = &$f['#entity'];
    $info = _entityspice_get_info($this->entityType);
    if ($info->hasUid()) {
      $entity->{$info->uidKey()} = $GLOBALS['user']->uid;
    }
    $entity = entity_save($this->entityType, $entity);
  }

}


// ____________________________________________________________________________


/**
 * Implements hook_entity_info().
 *
 * @param string $etype entity type
 *
 * @return mixed
 */
function _entityspice_entity_info($etype) {
  // entity_label() will suffice, thus the key not added.

  $info = _entityspice_get_info($etype);

  $ret_info[$etype] = [
    'module' => $info->module(),
    'label' => $info->humanName(),
    'controller class' => $info->controllerClass(),
    'base table' => $info->baseTable(),
    'revision table' => $info->revisionTable(),
    'uri callback' => $info->uriCallback(),
    'fieldable' => $info->isFieldable(),
    'entity keys' => [
      'id' => $info->idKey(),
      'revision' => $info->revisionKeyName(),
      'bundle' => $info->bundleKeyName(),
      'label' => $info->titleKey(),
    ],
    //    'bundle keys' => [ // TODO
    //      'bundle' => $info->bundleKeyName(),
    //    ],
    'bundles' => [], // TODO
    'view modes' => [ // TODO
      'full' => [
        'label' => t('Full'),
        'custom settings' => FALSE,
      ],
    ],
    'metadata controller class' => $info->metadataControllerClass(),
    'entity class' => $info->entityClass(),
    'views controller class' => $info->viewsControllerClass(),
    'token type' => $etype,
    'access callback' => $info->accessCallback(),
  ];
  $ret_info[$info->bundleMachineName()] = [
    'label' => $info->humanName() . ' Bundle',
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
      'controller class' => $info->bundleUIControllerClass(),
      'file' => $info->bundleAdminFile(),
      //         'file path' => $file_path,
    ],
  ];

  if (!$info->hasBundleAdminFile()) {
    unset($ret_info[$info->bundleMachineName()]['admin ui']['file']);
  }
  if (!$info->hasBundle()) {
    unset($ret_info[$info->bundleMachineName()]);
  }
  if (!$info->hasRevision()) {
    unset($ret_info[$etype]['revision table']);
    unset($ret_info[$etype]['entity keys']['revision']);
  }
  if (!$info->hasTitle()) {
    unset($ret_info[$etype]['entity keys']['label']);
  }

  return $ret_info;
}

/**
 * Executes a filter query against the provided attributes.
 *
 * Also used to filter entity list pages.
 *
 * TODO too damn complex. WTF?
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
 * Page callback for entity overview page.
 *
 * @param $etype string entity type.
 *
 * @return array
 * @throws \Exception
 */
function _entityspice_entity_page($etype) {
  // TODO critical, check _GET for safe input
  // Setup the header for both the query and table.
  $header = [
    'eid' => [
      'field' => 'eid',
      'type' => 'property',
      'data' => 'ID',
      'specifier' => 'eid',
    ],
    'title' => [
      'field' => 'title',
      'type' => 'property',
      'data' => 'Title',
      'specifier' => 'title',
    ],
    'owner' => [
      'field' => 'uid',
      'type' => 'property',
      'data' => 'Owner',
      'specifier' => 'uid',
    ],
  ];

  $info = _entityspice_get_info($etype);
  if ($info->hasBundle()) {
    $header['bundle'] = [
      'field' => 'bundle',
      'type' => 'property',
      'data' => 'Bundle',
      'specifier' => 'bundle',
    ];
  }
  if ($info->hasRevision()) {
    $header['updated'] = [
      'field' => 'updated',
      'type' => 'property',
      'data' => 'Updated',
      'sort' => 'desc',
      'specifier' => 'updated',
    ];
  }

  $header['operations'] = ['data' => t('Operations')];

  $result = FALSE;
  if (!isset($_POST['form_id'])) {
    $bundle = isset($_GET['bundle']) ? $_GET['bundle'] : '';
    $properties = isset($_GET['properties']) ? $_GET['properties'] : [];
    $fields = isset($_GET['fields']) ? $_GET['fields'] : [];
    $result = _entityspice_filter_query($etype, $header, $bundle, $properties, $fields);
  }

  $entities = [];
  if ($result) {
    $entities = entity_load($etype, array_keys($result[$etype]));
  }

  $rows = [];
  // @FIXME load "entity type" entity in a standard fashion.
  $callback = $etype . '_bundle_load';
  if (!empty($entities)) {
    $destination = drupal_get_destination();
    foreach ($entities as $id => $entity) {
      $uri = entity_uri($etype, $entity);
      $e_type = $callback($entity->bundle);
      $rows[$id] = [
        'eid' => $id,
        'title' => [
          'data' => [
            '#type' => 'link',
            '#title' => _entityspice_get_entity_name($etype, $entity),
            '#href' => $uri['path'],
          ],
        ],
        'owner' => [
          'data' => [
            '#theme' => 'username',
            '#account' => user_load($entity->uid),
          ],
        ],
        'type' => check_plain($e_type->name),
      ];

      if ($info->hasRevision()) {
        $rows[$id]['updated'] = format_date($entity->updated, 'short');
      }

      // Build a list of all the accessible operations for the current sec.
      $ops = [];
      if (entity_access('update', $etype, $entity)) {
        $ops['edit'] = [
          'title' => t('edit'),
          'href' => $uri['path'] . '/view/edit',
          'query' => $destination,
        ];
      }
      if (entity_access('delete', $etype, $entity)) {
        $ops['delete'] = [
          'title' => t('delete'),
          'href' => $uri['path'] . '/view/delete',
          'query' => $destination,
        ];
      }
      if (count($ops) > 1) {
        // Render an unordered list of operations links.
        $rows[$id]['operations'] = [
          'data' => [
            '#theme' => 'links__node_operations',
            '#links' => $ops,
            '#attributes' => ['class' => ['links', 'inline']],
          ],
        ];
      }
      elseif (!empty($ops)) {
        // Render the first and only operation as a link.
        $link = reset($ops);
        $rows[$id]['operations'] = [
          'data' => [
            '#type' => 'link',
            '#title' => $link['title'],
            '#href' => $link['href'],
            '#options' => ['query' => $link['query']],
          ],
        ];
      }
      else {
        unset($header['operations']);
      }
    }
  }

  $output = [
    'form' => drupal_get_form('entityspice_filter_form', $etype),
    'entities' => [
      '#theme' => 'entityspice_entity_list',
      '#entities' => $entities,
      '#entity_type' => $etype,
      '#header' => $header,
      '#rows' => $rows,
    ],
  ];
  return $output;
}

