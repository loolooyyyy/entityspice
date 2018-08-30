<?php print $a['php'] ?>

/**
* Implements hook_menu().
*/
function <?php echo $a['machine_name'] ?>_menu() {
$sinfo = _entityspice_get_info($etype);

  $etype = <?php echo $a['machine_name'] ?>;
  $einfo = entity_get_info($etype);
  $human_name = <?php echo $a['human name'] ?>;
  $root = <?php echo $a['parent userland path'] ?>;

  if(strlen($root) < 1) {
    throw new RuntimeException('base path must be at least one character');
  }

  $items[$root] = [
    'title' => $human_name,
    'description' => 'View ' . $human_name . 's',
    'page callback' => '_entityspice_entity_page',
    'page arguments' => [$etype],
    'access arguments' => ["administer $etype"],
  ];
  // TODO no bundle?
  $items[$root . '/add'] = [
    'title' => 'Add a ' . $human_name,
    'description' => 'Add a new ' . $human_name,
    'page callback' => '_entityspice_entity_bundles_list_page',
    'page arguments' => [$etype],
    'type' => MENU_NORMAL_ITEM,
    'access arguments' => ["add $etype"],
  ];

  <?php if($a['has bundle']) >
  foreach ($einfo['bundles'] as $bun_name => $bundle) {
    $items[$root . '/add/' . $bun_name] = [
      'title' => $bundle['label'],
      // TODO needed?
      'title callback' => 'check_plain',
      'description' => isset($bundle['description']) ? $bundle['description'] : '',
      'page callback' => '_entityspice_entity_add_page',
      'page arguments' => [$etype, $bun_name],
      'access arguments' => ['add ' . $etype . ' : ' . $bun_name],
    ];
  }
  <?php endif; >


  // Find where the character '%' will be in path, which represents the entity
  // id.

  if($root[0] === '/' || $root[strlen($root) - 1] === '/') {
    throw new RuntimeException('path must NOT begin with or end in slash.');
  }
  $root_parts = count(explode('/', $root));
  $arg_no = $root_parts + 1

  $access_cbk = <?php echo $a['safe'] . $a['machine_name'] ?> . '_entity_access';

  // TODO PERILS OF CHECK_PLAIN!!! the the fucking title is injectable.
  $items[$root . '/%'] = [
    'title callback' => '<?php echo $a['safe'] . $a['machine_name'] ?>_menu_title',
    'title arguments' => ['view', $arg_no],
    'page callback' => '<?php echo $a['safe'] . $a['machine_name'] ?>_entity_view',
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
    'title callback' => '<?php echo $a['safe'] . $a['machine_name'] ?>_menu_title',
    'title arguments' => ['edit', $arg_no],
    'page callback' => 'drupal_get_form',
    'page arguments' => [<?php echo $a['edit form'] ?>, $arg_no],
    'type' => MENU_LOCAL_TASK,
    'weight' => 2,
    'access callback' => $access_cbk,
    'access arguments' => ['edit', $arg_no],
  ];
  $items[$root . '/%/delete'] = [
    'title callback' => '<?php echo $a['safe'] . $a['machine_name'] ?>_menu_title',
    'title arguments' => ['delete', $arg_no],
    'page callback' => 'drupal_get_form',
    'page arguments' => [<?php echo $a['delete form'] ?>, $arg_no],
    'type' => MENU_LOCAL_TASK,
    'weight' => 5,
    'access callback' => $access_cbk,
    'access arguments' => ['delete', $arg_no],
  ];

  <?php if($a['devel support']) ?>
  if (module_exists('devel')) {
    $items[$root . '/%/devel'] = [
      'title' => 'Devel',
      'page callback' => '<?php echo $a['safe'] . $a['machine_name'] ?>_devel_load_object',
      'file' => 'inc/dev.inc',
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
      'page callback' => '<?php echo $a['safe'] . $a['machine_name'] ?>_devel_render_object',
      'file' => 'inc/dev.inc',
      'page arguments' => [$arg_no],
      'type' => MENU_LOCAL_TASK,
      'weight' => 100,
      'access arguments' => ['access devel information'],
    ];
  }
  <?php endif; ?>

  return $items;
}
`
/**
* Implements hook_admin_menu_map().

 TODO
*/
function __<?php echo $a['machine_name'] ?>_admin_menu_map() {
  $i = _entityspice_get_info($etype)['_entityspice_entity_info_alter'];
  $path = $i['path'];
  $name = '%' . $i['name'];

  $map[$path]['parent'] = $i['parent'];
  $map[$path]['arguments'][][$name] =
    _entityspice_entity_get_bundles_names($etype, FALSE);

  return $map;
}
