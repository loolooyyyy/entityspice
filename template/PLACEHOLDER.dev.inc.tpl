<?php print $a->php(); ?>

function <?php echo $a->sMachineName(); ?>_devel_load_object($object, $name = NULL) {
  $machine_name = '<?php echo $a->machineName(); ?>';

  if (!module_exists('devel')) {
    return drupal_not_found();
  }

  module_load_include('inc', 'devel', 'devel.pages');

  if(!is_object($entity)) {
    $entity = <?php echo $a->sMachineName(); ?>_load($entity);
  }
  if($entity === NULL) {
    return drupal_not_found();
  }

  return devel_load_object($machine_name, $object, $name);
}

/**
 * Page callback: wrapper for devel_render_object.
 *
 * @param $etype string entity type.
 * @param $object object entity object.
 * @param string $name name shown when displaying entity.
 *
 * @return mixed devel result
 */
function <?php echo $a->sMachineName(); ?>_devel_render_object($etype, $object, $name = NULL) {
  $machine_name = '<?php echo $a->machineName(); ?>';

  if (!module_exists('devel')) {
    return drupal_not_found();
  }

  module_load_include('inc', 'devel', 'devel.pages');

  if(!is_object($entity)) {
    $entity = <?php echo $a->sMachineName(); ?>_load($entity);
  }
  if($entity === NULL) {
    return drupal_not_found();
  }

  return devel_render_object($machine_name, $object, $name);
}
