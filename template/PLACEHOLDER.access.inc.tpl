<?php echo $a->php; ?>

<?php if($a->has_owner->value): ?>
/**
 * Check to see if $account is the owner of $entity.
 */
function <?php echo $a->s; ?>_entity_access_is_self($entity, $account) {
  if ($entity == NULL) {
    return FALSE;
  }

  // HANDLE CUSTOM SELF-ENTITY LOGIC HERE

  return FALSE;
}
<?php endif; ?>

/**
 * Check to see if $user has the $op access on $entity.
 *
<?php if($a->has_bundle): ?>
* If the entity is null, check if user has $op access on entity type or bundle.
<?php else: ?>
* If the entity is null, check if user has $op access on entity type.
<?php endif; ?>
 */
function <?php echo $a->s; ?>_entity_access($op,<?php if($a->has_bundle->value): ?> $bundle = NULL,<?php endif; ?> $entity = NULL, $user = NULL) {
  $access_controlled_actions = <?php echo $a->access_controlled_actions ?>;
  $machine_name = <?php echo $a->m ?>;

  if (!in_array($op, $access_controlled_actions) && $op !== "administer $machine_name") {
    return FALSE;
  }

  $account = NULL;
  if ($user !== NULL && is_object($user)) {
    $account = $user;
  }
  elseif ($user !== NULL && ctype_digit($user)) {
    $account = user_load($user);
  }
  elseif ($user === NULL) {
    $account = $GLOBALS['user'];
  }
  else {
    throw new RuntimeException('can not determine user');
  }

  if($account->uid === 1) {
    return TRUE;
  }

  if (user_access("administer $machine_name", $account)) {
    return TRUE;
  }

  if (user_access("$machine_name - $op", $account)) {
    return TRUE;
  }

<?php if($a->has_bundle->value): ?>
  if ($bundle !== NULL && user_access("$machine_name - $bundle - $op", $account)) {
    return TRUE;
  }
<?php endif; ?>

<?php if($a->has_owner->value): ?>
  $self = <?php echo $a->s; ?>_entity_access_is_self($entity, $account);
  if ($self && user_access("own - $machine_name - $op", $account)) {
    return TRUE;
  }

<?php if($a->has_bundle->value): ?>
  if ($self && $bundle !== NULL && user_access("own - $machine_name - $bundle - $op", $account)) {
    return TRUE;
  }
<?php endif; ?>
<?php endif; ?>

  return FALSE;
}

/**
 * Implements hook_permission().
 */
function <?php echo $a->m; ?>_permission() {
  $machine_name = '<?php echo $a->m; ?>';
  $actions = <?php echo $a->access_controlled_actions; ?>;
<?php if($a->has_bundle->value): ?>

  $bundles = <?php echo $a->s; ?>_get_bundles_names();
<?php endif; ?>

  $permissions = ["administer $machine_name"];

  foreach($actions as $op) {
    $permissions[] = "$machine_name - $op";
<?php if($a->has_owner->value): ?>
    $permissions[] = "own - $machine_name - $op";
<?php endif; ?>
  }

<?php if($a->has_bundle->value): ?>
  foreach($actions as $op) {
    foreach(<?php echo $a->s; ?>_get_bundles_names() as $bundle) {
      $permissions[] = "$machine_name - $bundle - $op";
<?php if($a->has_owner->value): ?>
      $permissions[] = "own - $machine_name - $bundle - $op";
<?php endif; ?>
    }
  }
<?php endif; ?>

  return $permissions;
}

