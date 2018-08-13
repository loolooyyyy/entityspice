<?php print $a['php'] ?>

/**
 * Implements hook_schema().
 */
function <?php echo $a['machine_name'] ?>_schema() {
  require_once drupal_get_path('module', 'entityspice') . '/entityspice.module';

  // Define custom entity fields here.
  $fields = [];
  $schema = _entityspice_entity_schema(<?php print $a['type_const'] ?>, $fields);

  return $schema;
}

/**
 * Implements hook_install().
 *
 * Create default bundle and default configuration.
 */
function <?php echo $a['machine_name'] ?>_install() {
 //if(!drupal_get_schema('<?php echo $a['machine_name'] ?>_bundle')) {
 //  return;
 //}
 //else {
 //  $bundle = entity_create('<?php echo $a['machine_name'] ?>_bundle', []);
 //  $bundle->locked = TRUE;
 //  $bundle->label = '<?php echo $a['label'] ?>';
 //  $bundle->name = '<?php echo $a['machine_name'] ?>';
 //  $bundle->save();
 //}
}
