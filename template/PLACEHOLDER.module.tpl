<?php print $a['php'] ?>

const <?php print $a['type_const'] ?> = '<?php print $a['machine_name'] ?>';

require_once dirname(__FILE__) . '/<?php echo $a['machine_name'] ?>.entity.inc';
require_once dirname(__FILE__) . '/<?php echo $a['machine_name'] ?>.menu.inc';
require_once dirname(__FILE__) . '/<?php echo $a['machine_name'] ?>.class.inc';
require_once dirname(__FILE__) . '/<?php echo $a['machine_name'] ?>.bundle.class.inc';
require_once dirname(__FILE__) . '/<?php echo $a['machine_name'] ?>.forms.inc';

class <?php echp $a['exception class'] > extends RuntimeException {
}

/**
 * Implements hook_permission().
 */
function <?php echo $a['machine_name'] ?>_permission() {
  $permissions = _entityspice_entity_permissions('<?php print $a['type_const'] ?>', '<?php print $a['label'] ?>');
  return $permissions;
}

function <?php echo $a['machine_name'] ?>_entity_access($op, $entity = NULL, $account = NULL) {
  $access = _entityspice_entity_access('<?php print $a['machine_name'] ?>', $op, $entity, $account);
  return $access;
}
