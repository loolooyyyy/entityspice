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
