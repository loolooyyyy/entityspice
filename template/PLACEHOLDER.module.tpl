<?php echo $a->php; ?>

require_once dirname(__FILE__) . '/<?php echo $a->m; ?>.access.inc';
require_once dirname(__FILE__) . '/<?php echo $a->m; ?>.bundle.inc';
require_once dirname(__FILE__) . '/<?php echo $a->m; ?>.class.inc';
require_once dirname(__FILE__) . '/<?php echo $a->m; ?>.entity.inc';
require_once dirname(__FILE__) . '/<?php echo $a->m; ?>.menu.inc';
// require_once dirname(__FILE__) . '/<?php echo $a->m; ?>.display.inc';

class <?php echo $a->uc; ?>Exception extends RuntimeException {
}

/**
 * Implements hook_entity_info().
 */
function <?php echo $a->m; ?>_entity_info() {
  $i['label'] = '<?php echo $a->human_name; ?>';
  $i['controller class'] = '<?php echo $a->uc; ?>EntityController';
  $i['base table'] = '<?php echo $a->base_table; ?>';
<?php if($a->has_revision->value): ?>
  $i['revision table'] = '<?php echo $a->revision_table; ?>';
<?php endif; ?>
<?php if($a->has_static_cache->value): ?>
  $i['static cache'] = TRUE;
<?php endif; ?>
<?php if($a->has_field_cache->value): ?>
    $i['field cache'] = TRUE;
<?php endif; ?>
//  $i['load hook'] = '<?php echo $a->m; ?>_load';
  $i['uri callback'] = '<?php echo $a->s; ?>_uri_callback';
  $i['label callback'] = '<?php echo $a->s; ?>_label_callback';
<?php if($a->is_fieldable->value): ?>
  $i['fieldable'] = TRUE;
<?php else: ?>
  $i['is_fieldable'] = FALSE;
<?php endif; ?>
<?php if($a->has_translation->value): ?>
  $i['translation']['locale'] = TRUE;
<?php endif; ?>
  $i['entity keys']['id'] = '<?php echo $a->id_key; ?>';
<?php if($a->has_revision->value): ?>
  $i['entity keys']['revision'] = '<?php echo $a->revision_key_name; ?>';
<?php endif; ?>
<?php if($a->has_bundle->value): ?>
  $i['entity keys']['bundle'] = '<?php echo $a->bundle_key_name; ?>';
<?php endif; ?>
<?php if($a->has_label_key->value): ?>
  $i['entity keys']['label'] = '<?php echo $a->label_key_name; ?>';
<?php endif; ?>
<?php if($a->has_language_key->value): ?>
  $i['entity keys']['language'] = 'language';
<?php endif; ?>

<?php if($a->has_bundle->value): ?>
  $i['bundle keys']['bundle'] = '<?php echo $a->bundle_name_key; ?>';
<?php endif; ?>

<?php if($a->has_bundle->value): ?>
  $default_bundles = <?php echo $a->default_bundles ?>;
  foreach ($default_bundles + <?php echo $a->s; ?>_get_bundles_options_list() as $bundle => $name) {
    $admin = '<?php echo $a->parent_admin_path; ?>/bundles/manage/%bundle';
    $real_admin = '<?php echo $a->parent_admin_path; ?>/bundles/manage/' . str_replace('_', '-', $bundle);
    $i['bundles'][$bundle]['label'] = $name;
    $i['bundles'][$bundle]['admin']['path'] = $admin;
    $i['bundles'][$bundle]['admin']['real path'] = $real_admin;
    $i['bundles'][$bundle]['admin']['bundle argument'] = <?php echo _entityspice_calculate_bundle_argument($a->parent_admin_path->value . '/bundle/manage/%bun') ?>;
    $i['bundles'][$bundle]['admin']['access argument'] = ['administer site configuration'];
  }
<?php endif; ?>

  $view_modes = <?php echo $a->view_modes ?>;
  foreach($view_modes as $mode => $mode_label) {
    $i['view modes'][$mode]['label'] = t($mode_label);
    $i['view modes'][$mode]['custom settings'] = TRUE;
  }

  return ['<?php echo $a->m ?>' => $i];
}

