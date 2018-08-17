<?php

// _______________________________________________________________________ MENU

/**
 * implementation for hook_menu().
 *
 * @param $etype string entity type.
 *
 * @return mixed
 * @throws \Exception
 */
function _entityspice_entity_menu_items($etype) {
  $sinfo = _entityspice_get_info($etype);
  $einfo = entity_get_info($etype);

  $root = $sinfo->parentUserlandPath();

  $items[$root] = [
    'title' => $sinfo->humanName(),
    'description' => 'View ' . $sinfo->humanName() . 's',
    'page callback' => '_entityspice_entity_page',
    'page arguments' => [$etype],
    'access arguments' => ["administer $etype"],
  ];
  $items[$root . '/add'] = [
    'title' => 'Add a ' . $sinfo->humanName(),
    'description' => 'Add a new ' . $sinfo->humanName(),
    'page callback' => '_entityspice_entity_bundles_list_page',
    'page arguments' => [$etype],
    'type' => MENU_NORMAL_ITEM,
    'access arguments' => ['add ' . $etype],
  ];

  foreach ($einfo['bundles'] as $bun_name => $bundle) {
    $items[$root . '/add/' . $bun_name] = [
      'title' => $bundle['label'],
      'title callback' => 'check_plain',
      'description' => isset($bundle['description']) ? $bundle['description'] : '',
      'page callback' => '_entityspice_entity_add_page',
      'page arguments' => [$etype, $bun_name],
      'access arguments' => ['add ' . $etype . ' : ' . $bun_name],
    ];
  }

  $arg_no = $sinfo['parent user path arg no'];
  $access_cbk = $etype . '_entity_access';

  $items[$root . '/%'] = [
    'title callback' => '_entityspice_entity_title',
    'title arguments' => [$etype, $arg_no],
    'page callback' => '_entityspice_entity_view',
    'page arguments' => [$etype, $arg_no],
    'weight' => 10,
    'access callback' => $access_cbk,
    'access arguments' => ['view', $arg_no],
  ];
  $items[$root . '/%/view'] = [
    'title' => 'Summary',
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => -10,
    'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
    'access callback' => $access_cbk,
    'access arguments' => ['view', $arg_no],
  ];
  $items[$root . '/%/view/edit'] = [
    'title' => 'Edit',
    'page callback' => 'drupal_get_form',
    'page arguments' => [$sinfo->editForm(), $arg_no],
    'type' => MENU_LOCAL_TASK,
    'weight' => 2,
    'access callback' => $access_cbk,
    'access arguments' => ['edit', $arg_no],
  ];
  $items[$root . '/%/view/delete'] = [
    'title' => 'Delete',
    'page callback' => 'drupal_get_form',
    'page arguments' => [$sinfo->deleteForm(), $arg_no],
    'type' => MENU_LOCAL_TASK,
    'weight' => 5,
    'access callback' => $access_cbk,
    'access arguments' => ['delete', $arg_no],
  ];

  if (module_exists('devel')) {
    $items[$root . '/%/devel'] = [
      'title' => 'Devel',
      'page callback' => '_entityspice_devel_load_object',
      'file' => 'inc/dev_datahandler.class.inc',
      'page arguments' => [$etype, $arg_no],
      'type' => MENU_LOCAL_TASK,
      'weight' => 100,
      'access arguments' => ['access devel information'],
    ];
    $items[$root . '/%/devel/load'] = [
      'title' => 'Load',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'access arguments' => ['access devel information'],
    ];
    $items[$root . '/%/devel/render'] = [
      'title' => 'Render',
      'page callback' => '_entityspice_devel_render_object',
      'file' => 'inc/dev_datahandler.class.inc',
      'page arguments' => [$etype, $arg_no],
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

/**
 * implementation for hook_admin_menu_map().
 *
 * @param $etype string entity type.
 *
 * @return array admin menu map
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
 *
 * @param $etype string entity type
 * @param $entity object/int entity id or entity object with an id.
 *
 * @return bool true if entity of etype exists with the given id.
 */
function _entityspice_entity_exists_in_db($etype, $entity) {
  $id = is_object($entity) ? $entity->getID() : $entity;
  return ctype_digit($id) && entity_load($etype, [$id]);
}

// _______________________________________________________________________ PAGE

/**
 * Page callback for adding an entity.
 *
 * Return a list of available bundles to add or the add entity page if there's
 * only a single bundle.
 *
 * @param $etype string entity type.
 *
 * @return string themed output of bundles list for an entity type.
 * @throws \Exception them() throws exception.
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
 *
 * @param $etype string entity type.
 * @param array $overrides overridden values when getting entity type info.
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

/**
 * Page callback for adding an entity.
 *
 * @param $etype string entity type
 * @param $bundle string bundle of entity being added.
 * @param array $overrides overridden values when getting entity type info.
 *
 * @return array entity add form.
 * @throws \Exception
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

