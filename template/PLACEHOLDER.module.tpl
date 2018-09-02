<?php print $a->php(); ?>

require_once dirname(__FILE__) . '/<?php echo $a->machineName(); ?>.entity.inc';
require_once dirname(__FILE__) . '/<?php echo $a->machineName(); ?>.menu.inc';
require_once dirname(__FILE__) . '/<?php echo $a->machineName(); ?>.class.inc';
require_once dirname(__FILE__) . '/<?php echo $a->machineName(); ?>.bundle.class.inc';
require_once dirname(__FILE__) . '/<?php echo $a->machineName(); ?>.forms.inc';

class <?php echo $a->camelNameUcFirst(); ?>Exception extends RuntimeException {
}

/**
 * Implements hook_permission().
 */
function <?php echo $a->machineName(); ?>_permission() {
<?php if($a->hasBundle()): ?>
<?php else: ?>
<?php endif; ?>
  $machine_name = '<?php echo $a->machineName(); ?>';
  $actions = '<?php implode(', ', $a->accessControlledActions()); ?>';
  $permissions = [];
  return $permissions;
}
