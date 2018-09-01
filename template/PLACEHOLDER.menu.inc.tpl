<?php print $a['php'] ?>

/**
* Implements hook_menu().
*/
function <?php echo $a['machine_name'] ?>_menu() {
  $machine_name = '<?php echo $a['machine_name'] ?>';
  $s_name = '<?php echo $a['safe'] . $a['machine_name'] ?>';
  $human_name = '<?php echo $a['human name'] ?>';
  $root = '<?php echo $a['parent userland path'] ?>';
  $einfo = entity_get_info($machine_name);

  if(strlen($root) < 1) {
    throw new RuntimeException('base path must be at least one character');
  }

  // NOT IMPLEMENTED, use views.
  // $items[$root] = [
  //   'title' => $human_name,
  //   'description' => 'View ' . $human_name . 's',
  //   'page callback' => $s_name . '_entity_page',
  //   'access arguments' => ["administer $machine_name"],
  // ];

  // TODO no bundle?
  $items[$root . '/add'] = [
    'title' => 'Add a ' . $human_name,
    'description' => 'Add a new ' . $human_name,
<?php if($a['has bundle']): ?>
    'page callback' => $s_name . '_bundles_list_page',
<?php else: ?>
    'page callback' => $s_name . '_add_page',
<?php endif; ?>
    'type' => MENU_NORMAL_ITEM,
    'access arguments' => ["add $machine_name"],
  ];

  <?php if($a['has bundle']): ?>
  foreach ($einfo['bundles'] as $bun_name => $bundle) {
    $items[$root . '/add/' . $bun_name] = [
      'title' => $bundle['label'],
      // TODO needed?
      'title callback' => 'check_plain',
      'description' => isset($bundle['description']) ? $bundle['description'] : '',
      'page callback' => $s_name . '_add_page',
      'page arguments' => [$bun_name],
      'access arguments' => ['add ' . $machine_name . ' : ' . $bun_name],
    ];
  }
  <?php endif; ?>


  // Find where the character '%' will be in path, which represents the entity
  // id.

  if($root[0] === '/' || $root[strlen($root) - 1] === '/') {
    throw new RuntimeException('path must NOT begin with or end in slash.');
  }
  $root_parts = count(explode('/', $root));
  $arg_no = $root_parts + 1

  $access_cbk = $s_name . '_entity_access';

  // TODO PERILS OF CHECK_PLAIN!!! the the fucking title is injectable.
  $items[$root . '/%'] = [
    'title callback' => $s_name . '_menu_title',
    'title arguments' => ['view', $arg_no],
    'page callback' => $s_name . '_entity_view',
    'page arguments' => [$arg_no],
    'weight' => 10,
    'access callback' => $access_cbk,
    'access arguments' => ['view', $arg_no],
  ];
  $items[$root . '/%/view'] = $items[$root . '/%'];
  // 'type' => MENU_DEFAULT_LOCAL_TASK,
  // 'weight' => -10,
  // 'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,

  $items[$root . '/%/edit'] = [
    'title callback' => $s_name . '_menu_title',
    'title arguments' => ['edit', $arg_no],
    'page callback' => 'drupal_get_form',
    'page arguments' => ['<?php echo $a['edit form'] ?>', $arg_no],
    'type' => MENU_LOCAL_TASK,
    'weight' => 2,
    'access callback' => $access_cbk,
    'access arguments' => ['edit', $arg_no],
  ];
  $items[$root . '/%/delete'] = [
    'title callback' => $s_name . '_menu_title',
    'title arguments' => ['delete', $arg_no],
    'page callback' => 'drupal_get_form',
    'page arguments' => ['<?php echo $a['delete form'] ?>', $arg_no],
    'type' => MENU_LOCAL_TASK,
    'weight' => 5,
    'access callback' => $access_cbk,
    'access arguments' => ['delete', $arg_no],
  ];

  <?php if($a['devel support']): ?>
  if (module_exists('devel')) {
    $items[$root . '/%/devel'] = [
      'title' => 'Devel',
      'page callback' => $s_name . '_devel_load_object',
      'file' => '<?php echo $a['machine_name']; ?>.dev.inc',
      'page arguments' => [$arg_no],
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
      'page callback' => $s_name . '_devel_render_object',
      'file' => '<?php echo $a['machine_name']; ?>.dev.inc',
      'page arguments' => [$arg_no],
      'type' => MENU_LOCAL_TASK,
      'weight' => 100,
      'access arguments' => ['access devel information'],
    ];
  }
  <?php endif; ?>

  return $items;
}

/**
 * Implements hook_menu_local_tasks_alter().
 *
 * Changing the BASEPATH/ENTITY/add menu item to type MENU_LOCAL_ACTION would
 * break display of child items. Instead, dynamically add the local action.
 *
 * Adds action link to 'xxxx/yyyy/add' on 'xxxx/yyyyy' page.
 */
function <?php echo $a['machine_name'] ?>_menu_local_task_alter(&$data, $router_item, $root_path) {
  $machine_name = NULL;
  $parent_admin_path = '';
  if ($parent_admin_path === $root_path) {
    $item = menu_get_item($parent_admin_path . '/add');
    if ($item['access']) {
      $data['actions']['output'][] = [
        '#theme' => 'menu_local_action',
        '#link' => $item,
      ];
    }
  }
}



/**
* Implements hook_admin_menu_map().
 TODO
*/
function _<?php echo $a['machine_name'] ?>_admin_menu_map() {
  $i = _entityspice_get_info($machine_name)['_entityspice_entity_info_alter'];
  $path = $i['path'];
  $name = '%' . $i['name'];

  $map[$path]['parent'] = $i['parent'];
  $map[$path]['arguments'][][$name] =
    <?php echo $a['safe'] . $a['machine_name'] ?>_get_bundles_names();

  return $map;
}


// ______________________________________________________________________ PAGES


/**
 * Page callback for adding an entity.
 *
 * Return a list of available bundles to add or the add entity page if there's
 * only a single bundle.
 *
 * @return string themed output of bundles list.
 */
function <?php echo $a['safe'] . $a['machine_name'] ?>_bundles_list_page() {
  $s_name = '<?php echo $a['safe'] . $a['machine_name'] ?>';
  $item = menu_get_item();
  $content = system_admin_menu_block($item);

  // Bypass the entity/add listing if only one entity bundle is available.
  if (count($content) == 1) {
    $item = array_shift($content);
    drupal_goto($item['href']);
  }

  $vars = ['content' => $content];
  $output = theme($s_name . '_entity_add_list', $vars);

  return $output;
}

/**
 * Page callback for entity overview page.
 */
function  <?php echo $a['safe'] . $a['machine_name'] ?>_entity_page() {
  // TODO a copy of admin/node, but there are too much custom properties and
  // optional ones too, so use views anyway.
  throw new RuntimeException('unsupported');
}

/**
* Page callback for adding an entity.
*/
function <?php echo $a['safe'] . $a['machine_name']; ?>_add_page(<?php if($a['has bundle']): ?>$bundle<?php endif ?>) {
  $s_name = '<?php echo $a['safe'] . $a['machine_name'] ?>';
  $module = '<?php echo $a['module'] ?>';

  $form_callback = ;

  module_load_include($module, 'forms.inc');

  $entity = <?php echo $a['safe'] . $a['machine_name']; ?>_entity_create(<?php if($a['has bundle']): ?>['bundle' => $bundle]<?php endif; ?>);
  return drupal_get_form("<?php echo $info['add form']; ?>", $entity);
}
