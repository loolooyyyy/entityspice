<?php print $a->php; ?>

require_once dirname(__FILE__) . '/<?php echo $a->m; ?>.access.inc';
require_once dirname(__FILE__) . '/<?php echo $a->m; ?>.admin.inc';
require_once dirname(__FILE__) . '/<?php echo $a->m; ?>.bundle.class.inc';
require_once dirname(__FILE__) . '/<?php echo $a->m; ?>.class.inc';
require_once dirname(__FILE__) . '/<?php echo $a->m; ?>.display.inc';
require_once dirname(__FILE__) . '/<?php echo $a->m; ?>.entity.inc';
require_once dirname(__FILE__) . '/<?php echo $a->m; ?>.menu.inc';

class <?php echo $a->uc; ?>Exception extends RuntimeException {
}

/**
 * Implements hook_permission().
 */
function <?php echo $a->m; ?>_permission() {
  $machine_name = '<?php echo $a->m; ?>';
  $actions = '<?php print $a->access_controlled_actions->value; ?>';

  $bundles = <?php echo $a->s; ?>_get_bundles_names();
  $permissions = [];

  foreach($actions as $op) {
    $permissions[] = "$machine_name - $op";
    $permissions[] = "own - $machine_name - $op";
  }

<?php if($a->has_bundle->value): ?>
  foreach($actions as $op) {
    foreach(<?php echo $a->s; ?>_get_bundles_names() as $bundle) {
      $permissions[] = "$machine_name - $bundle - $op";
      $permissions[] = "own - $machine_name - $bundle - $op";
    }
  }
<?php endif; ?>

  return $permissions;
}

/**
 * Implements hook_entity_info().
 */
function <?php echo $a->m; ?>_entity_info() {
  $i['label'] = '<?php print $a->human_name->value; ?>';
  $i['controller class'] = '<?php print $a->uc; ?>EntityController';
  $i['base table'] = '<?php print $a->base_table->value; ?>';
<?php if($a->has_revision->value): ?>
  $i['revision table'] = '<?php print $a->revision_table->value; ?>';
<?php endif; ?>
  $i['static cache'] = '<?php print $a->has_static_cache->value; ?>';
  $i['field cache'] = '<?php print $a->has_field_cache->value; ?>';
  $i['load hook'] = '<?php print $a->m; ?>_load';
  $i['uri callback'] = '<?php print $a->s; ?>_uri_callback';
  $i['label callback'] = '<?php print $a->s; ?>_label_callback';
  $i['is_fieldable'] = '<?php print $a->is_fieldable->value; ?>';
<?php if($a->has_translation->value): ?>
  $i['translation']['locale'] = TRUE;
<?php endif; ?>
  $i['entity keys']['id'] = '<?php print $a->id_key->value; ?>';
<?php if($a->has_revision->value): ?>
  $i['entity keys']['revision'] = '<?php print $a->revision_key_name->value; ?>';
<?php endif; ?>
<?php if($a->has_bundle->value): ?>
  $i['entity keys']['bundle'] = '<?php print $a->bundle_key_name->value; ?>';
<?php endif; ?>
<?php if($a->has_label_key->value): ?>
  $i['entity keys']['label'] = '<?php print $a->label_key_name->value; ?>';
<?php endif; ?>
<?php if($a->has_language_key->value): ?>
  $i['entity keys']['language'] = 'language';
<?php endif; ?>

<?php if($a->has_bundle->value): ?>
  $i['bundle keys']['bundle'] = 'name';
<?php endif; ?>

<?php if($a->has_bundle->value): ?>
  $default_bundles = [<?php foreach($a->default_bundles->value as $bun_machine => $bun_label): ?>
    '<?php echo $bun_machine; ?>' => '<?php echo $bun_label; ?>',
  <?php endforeach; ?>];
  foreach ($default_bundles + <?php echo $a->s; ?>_get_bundles_options_list() as $bundle => $name) {
    $admin = '<?php echo $a->parent_admin_path->value; ?>/bundles/manage/%bundle';
    $real_admin = '<?php echo $a->parent_admin_path->value; ?>/bundles/manage/' . str_replace('_', '-', $bundle);
    $i['bundles'][$bundle]['label'] = $name;
    $i['bundles'][$bundle]['admin']['path'] = $admin;
    $i['bundles'][$bundle]['admin']['real path'] = $real_admin;
    $i['bundles'][$bundle]['admin']['bundle argument'] = <?php echo _entityspice_calculate_bundle_argument($a->parent_admin_path->value . '/bundle/manage/%bun') ?>;
    $i['bundles'][$bundle]['admin']['access argument'] = ['administer site configuration'];
  }
<?php endif; ?>

  $view_modes = [<?php foreach($a->view_modes->value as $v_name => $v_label): ?>
    '<?php echo $v_name; ?>' => '<?php echo $v_label; ?>',
  <?php endforeach; ?>];
  foreach($view_modes as $mode => $mode_label) {
    $i['view modes'][$mode]['label'] = t($mode_label);
    $i['view modes'][$mode]['custom settings'] = TRUE;
  }

  return $i;
}

