<?php

// ______________________________________________________________________ DEVEL

/**
 * Page callback: wrapper for devel_load_object.
 */
function _entityspice_devel_load_object($etype, $object, $name = NULL) {
  $object = _entityspice_get_entity($etype, $object);
  if (module_exists('devel')) {
    module_load_include('inc', 'devel', 'devel.pages');
    return devel_load_object($etype, $object, $name);
  }
  else {
    return drupal_not_found();
  }
}

/**
 * Page callback: wrapper for devel_render_object.
 */
function _entityspice_devel_render_object($etype, $object, $name = NULL) {
  $object = _entityspice_get_entity($etype, $object);
  if (module_exists('devel')) {
    module_load_include('inc', 'devel', 'devel.pages');
    return devel_render_object($etype, $object, $name);
  }
  else {
    return drupal_not_found();
  }
}

// _______________________________________________________________________ MENU

/**
 * implementation of hook_menu().
 */
function _entityspice_entity_menu_items($etype) {
  $info = _entityspice_get_info($etype);

  $label = $info['label'];
  $parent_path_u = $info['parent userland path'];

  $edit_form = $info['edit form'];
  $delete_form = $info['delete form'];

  $items[$parent_path_u] = [
    'title' => $label,
    'description' => 'View ' . $label . 's',
    'page callback' => '_entityspice_entity_page',
    'page arguments' => [$etype],
    'access arguments' => ["administer $etype"],
  ];

  $items[$parent_path_u . '/add'] = [
    'title' => 'Add a ' . $label,
    'description' => 'Add a new ' . $label,
    'page callback' => '_entityspice_entity_bundles_list_page',
    'page arguments' => [$etype],
    'type' => MENU_NORMAL_ITEM,
    'access arguments' => ['add ' . $etype],
  ];

  $entity_info = entity_get_info($etype);
  foreach ($entity_info['bundles'] as $bun_name => $bundle) {
    $items[$parent_path_u . '/add/' . $bun_name] = [
      'title' => $bundle['label'],
      'title callback' => 'check_plain',
      'description' => isset($bundle['description']) ? $bundle['description'] : '',
      'page callback' => '_entityspice_entity_add_page',
      'page arguments' => [$etype, $bun_name],
      'access arguments' => ['add ' . $etype . ' : ' . $bun_name],
    ];
  }

  $argno = $info['parent user path arg no'];
  $access_cbk = $etype . '_entity_access';
  $items[$parent_path_u . '/%'] = [
    'title callback' => '_entityspice_entity_title',
    'title arguments' => [$etype, $argno],
    'page callback' => '_entityspice_entity_view',
    'page arguments' => [$etype, $argno],
    'weight' => 10,
    'access callback' => $access_cbk,
    'access arguments' => ['view', $argno],
  ];

  $items[$parent_path_u . '/%/view'] = [
    'title' => 'Summary',
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => -10,
    'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
    'access callback' => $access_cbk,
    'access arguments' => ['view', $argno],
  ];

  $items[$parent_path_u . '/%/view/edit'] = [
    'title' => 'Edit',
    'page callback' => 'drupal_get_form',
    'page arguments' => [$edit_form, $argno],
    'type' => MENU_LOCAL_TASK,
    'weight' => 2,
    'access callback' => $access_cbk,
    'access arguments' => ['edit', $argno],
  ];

  $items[$parent_path_u . '/%/view/delete'] = [
    'title' => 'Delete',
    'page callback' => 'drupal_get_form',
    'page arguments' => [$delete_form, $argno],
    'type' => MENU_LOCAL_TASK,
    'weight' => 5,
    'access callback' => $access_cbk,
    'access arguments' => ['delete', $argno],
  ];

  if (module_exists('devel')) {
    $items[$parent_path_u . '/%/devel'] = [
      'title' => 'Devel',
      'page callback' => '_entityspice_devel_load_object',
      'page arguments' => [$etype, $argno],
      'type' => MENU_LOCAL_TASK,
      'weight' => 100,
      'access arguments' => ['access devel information'],
    ];

    $items[$parent_path_u . '/%/devel/load'] = [
      'title' => 'Load',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'access arguments' => ['access devel information'],
    ];

    $items[$parent_path_u . '/%/devel/render'] = [
      'title' => 'Render',
      'page callback' => '_entityspice_devel_render_object',
      'page arguments' => [$etype, $argno],
      'type' => MENU_LOCAL_TASK,
      'weight' => 100,
      'access arguments' => ['access devel information'],
    ];
  }

  return $items;
}

/**
 * Why did I put this here? what does these comments mean? :|
 *
 * Implements hook_menu_local_tasks_alter().
 *
 * Changing the BASEPATH/ENTITY/add menu item to type MENU_LOCAL_ACTION would
 * break display of child items. Instead, dynamically add the local action.
 *
 * Adds action link to 'xxxx/yyyy/add' on 'xxxx/yyyyy' page.
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

/**
 * implementation of hook_admin_menu_map().
 */
function _entityspice_admin_menu_map($etype) {
  $i = _entityspice_get_info($etype)['_entityspice_entity_info_alter'];
  $path = $i['path'];
  $name = '%' . $i['name'];

  $map[$path]['parent'] = $i['parent'];
  $map[$path]['arguments'][][$name] =
    _entityspice_entity_get_bundles_names($etype, FALSE);

  return $map;
}

/**
 * Makes sure entity exists.
 *
 * Is called in menu callbacks, like entity delete form or entity view page, to
 * make sure asked entity actually exists.
 */
function _entityspice_entity_in_path_exists($etype, $entity) {
  $id = is_object($entity) ? $entity->getID() : $entity;
  return ctype_digit($id) && entity_load($etype, [$id]);
}

// _______________________________________________________________________ PAGE

/**
 * Page callback for adding an entity.
 *
 * Return a list of available entities to add or the add entity page if there's
 * only a single entity type.
 */
function _entityspice_entity_bundles_list_page($etype) {
  $item = menu_get_item();
  $content = system_admin_menu_block($item);

  // Bypass the entity/add listing if only one entity bundle is available.
  if (count($content) == 1) {
    $item = array_shift($content);
    drupal_goto($item['href']);
  }

  $vars = ['content' => $content, 'entity_type' => $etype];
  $output = theme('entityspice_entity_add_list', $vars);

  return $output;
}

/**
 * Page callback for entity overview page.
 */
function _entityspice_entity_page($etype, array $overrides = []) {
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

  $info = _entityspice_get_info_override($etype, $overrides);
  $has_bundle = $info['has bundle'];
  $has_revision = $info['has_revision'];

  if ($has_bundle) {
    $header['bundle'] = [
      'field' => 'bundle',
      'type' => 'property',
      'data' => 'Bundle',
      'specifier' => 'bundle',
    ];
  }

  if ($has_revision) {
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

      if ($has_revision) {
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

/**
 * Page callback for adding an entity.
 */
function _entityspice_entity_add_page($etype, $bundle, array $overrides = []) {
  $info = _entityspice_get_info($etype);

  $module = isset($overrides['module']) ? $overrides['module'] : $info['module'];
  $form_callback = isset($overrides['form callback']) ? $overrides['add form'] : $info['add form'];

  if (isset($overrides['form file'])) {
    $form_file = $overrides['form file'];
  }
  else {
    $dir = $info['forms file']['directory'] ? '/' . $info['forms file']['directory'] : '/';
    $file = $info['forms file']['file'];
    $suffix = $info['forms file']['suffix'];
    $form_file = $dir . $file . '.' . $suffix;
  }

  require_once drupal_get_path('module', $module) . $form_file;


  $entity = _entityspice_entity_create($etype, ['bundle' => $bundle]);
  return drupal_get_form($form_callback, $entity);
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

