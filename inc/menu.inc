<?php

/**
 * Why did I put this here? what does these comments mean? :|
 *
 * Implements hook_menu_local_tasks_alter().
 *
 * Changing the BASEPATH/ENTITY/add menu item to type MENU_LOCAL_ACTION would
 * break display of child items. Instead, dynamically add the local action.
 *
 * Adds action link to 'xxxx/yyyy/add' on 'xxxx/yyyyy' page.
 *
 * @param $etype entity type
 * @param $data
 * @param $router_item
 * @param $root_path
 *
 * @throws \Exception
 */
function _entityspice_menu_local_task_alter($etype, &$data, $router_item, $root_path) {
  if ($path = _entityspice_get_info($etype)['parent admin path'] == $root_path) {
    $item = menu_get_item($path . '/add');
    if ($item['access']) {
      $data['actions']['output'][] = [
        '#theme' => 'menu_local_action',
        '#link' => $item,
      ];
    }
  }
}

// _______________________________________________________________________ PAGE

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

// ______________________________________________________________________ THEME

/**
 * Implements hook_theme().
 *
 * Purpose:
 *   - Defines theme callback for 'add entity ...' page.
 *   - Defines theme callback for property field of entity (pseudo fields).
 *   - Defines theme callback for default ES page '/es'.
 */
function entityspice_theme() {
  $info = [
    'entityspice_entity_add_list' => ['content', 'entity_type'],
    'entityspice_entity_list' => ['header', 'rows', 'entity_type', 'entities'],
    'entityspice_default_page' => ['entity_types'],
  ];

  $theme = [];
  foreach ($info as $t_name => $t_vars) {
    foreach ($t_vars as $var) {
      $theme[$t_name]['variables'][$var] = NULL;
    }
  }

  $theme['entityspice_entity_property_field'] = [
    'variables' => [
      'label_hidden' => FALSE,
      'title_attributes' => NULL,
      'label' => '',
      'content_attributes' => NULL,
      'items' => [],
      'item_attributes' => [0 => ''],
      'classes' => '',
      'attributes' => '',
    ],
  ];


  return $theme;
}

/**
 * Returns HTML for a list of available node types for node creation.
 *
 * @param array $variables
 *   An associative array containing:
 *   - content: An array of content types.
 *
 * @ingroup themeable
 * @return string
 */
function theme_entityspice_entity_add_list($variables) {
  $content = $variables['content'];
  $etype = $variables['entity_type'];
  $info = _entityspice_get_info($etype);
  $bundles_page = $info['parent admin path'];

  if ($content) {
    $output = '<dl class="node-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l($item['title'], $item['href'], $item['localized_options']) . '</dt>';
      $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
    }
    $output .= '</dl>';
  }
  else {
    $args = [
      '@create-bundle' => url($bundles_page),
      '@etype' => $variables['entity_type'],
    ];
    $output = '<p>';
    $output .= t('You have not created any @etype yet. Go to the <a href="@create-bundle"> bundle administration page</a> to add a new @etype bundle.', $args);
    $output .= '</p>';
  }

  return $output;
}

/**
 * Theme function for entity list.
 */
function theme_entityspice_entity_list($variables) {
  $header = $variables['header'];
  $rows = $variables['rows'];
  $etype = $variables['entity_type'];

  $render['table'] = [
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No @etype entities available.', ['@etype' => $etype]),
  ];
  $render['pager'] = ['#theme' => 'pager'];

  return render($render);
}

/**
 * Returns HTML for a list of available entity types for entity creation.
 *
 * @param array $variables
 *   An associative array containing:
 *   - entity_types: An array of entity types.
 *
 * @ingroup themeable
 * @return string
 */
function theme_entityspice_default_page($variables) {
  $content = $variables['content'];

  if ($content) {
    $output = '<dl class="node-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l($item['title'], $item['href'], $item['localized_options']) . '</dt>';
      $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
    }
    $output .= '</dl>';
  }
  else {
    $output = '<p>';
    $output .= t('You have not created any entity types yet. Use drush to generate one.');
    $output .= '</p>';
  }

  return $output;
}

/**
 * Theme function for entity properties.
 *
 * Simple wrapper around theme_field that sets default values and ensures
 * properties render consistently with fields.
 *
 * @throws \Exception
 */
function theme_entityspice_entity_property_field($variables) {
  return theme('field', $variables);
}

