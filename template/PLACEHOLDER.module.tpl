<?php print $a->php ?>

require_once dirname(__FILE__) . '/<?php echo $a->m; ?>.entity.inc';
require_once dirname(__FILE__) . '/<?php echo $a->m; ?>.menu.inc';
require_once dirname(__FILE__) . '/<?php echo $a->m; ?>.class.inc';
require_once dirname(__FILE__) . '/<?php echo $a->m; ?>.bundle.class.inc';
require_once dirname(__FILE__) . '/<?php echo $a->m; ?>.forms.inc';

class <?php echo $a->uc; ?>Exception extends RuntimeException {
}

/**
 * Implements hook_permission().
 */
function <?php echo $a->m; ?>_permission() {
<?php if($a->has_bundle->value): ?>
<?php else: ?>
<?php endif; ?>
  $machine_name = '<?php echo $a->m; ?>';
  $actions = '<?php print $a->access_controlled_actions->value; ?>';
  $permissions = [];
  return $permissions;
}

/**
 * Implements hook_entity_info().
 * TODO has_language
   TODO translation (module)
   TODO language on entity keys
   TODO view modes
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
  $i['is_fieldabe'] = '<?php print $a->is_fieldable->value; ?>';
  $i['entity keys']['id'] = '<?php print $a->id_key_name->value; ?>';
<?php if($a->has_revision->value): ?>
  $i['entity keys']['revision'] = '<?php print $a->revision_key_name->value; ?>';
<?php endif; ?>
<?php if($a->has_bundle->value): ?>
  $i['entity keys']['bundle'] = '<?php print $a->bundle_key_name->value; ?>';
<?php endif; ?>
<?php if(!$a->has_label_callback->value && $a->has_label_key->value): ?>
  $i['entity keys']['label'] = '<?php print $a->label_key_name->value; ?>';
<?php endif; ?>

<?php if($a->has_bundle->value): ?>
  $i['bundle keys']['name'] = 'name';
<?php endif; ?>

<?php if($a->has_bundle->value): ?>
  foreach (<?php echo $a->s; ?>_get_bundles_options_list() as $bundle => $name) {
    $admin = '<?php echo $a->parent_admin_path->value; ?>/bundles/manage/%bundle';
    $real_admin = '<?php echo $a->parent_admin_path->value; ?>/bundles/manage/' . str_replace('_', '-', $bundle);
    $i['bundles'][$bundle]['label'] = $name;
    $i['bundles'][$bundle]['admin']['path'] = $admin;
    $i['bundles'][$bundle]['admin']['real path'] = $real_admin;
    $i['bundles'][$bundle]['admin']['bundle argument'] = <?php echo _entityspice_calculate_bundle_argument($a->parent_admin_path->value . '/bundle/manage/%bun') ?>;
    $i['bundles'][$bundle]['admin']['access argument'] = ['administer site configuration'];
  }
<?php endif; ?>

  return $i;
}

