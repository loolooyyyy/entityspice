<?php print $a->php; ?>

/**
 * Check to see if $account is the owner of $entity.
 */
function <?php echo $a->sMachineName(); ?>_entity_access_is_self($entity, $account) {
  if ($entity == NULL) {
    return FALSE;
  }

  // HANDLE CUSTOM SELF-ENTITY LOGIC HERE

  return FALSE;
}


/**
 * Check to see if $user has the $op access on $entity.
 *
<?php if($a->hasBundle()): ?>
* If the entity is null, check if user has $op access on entity type or bundle.
<?php else: ?>
* If the entity is null, check if user has $op access on entity type.
<?php endif; ?>
 */
function <?php echo $a->sMachineName(); ?>_entity_access($op,<?php if($a->hasBundle()): ?> $bundle = NULL,<?php endif; ?> $entity = NULL, $user = NULL) {
  $access_controlled_actions = NULL;
  $machine_name = NULL;

  if (!in_array($op, $access_controlled_actions)) {
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

  if (user_access("$machine_name - $op", $account)) {
    return TRUE;
  }

<?php if($a->hasBundle()): ?>
  if ($bundle !== NULL && user_access("$machine_name - $bundle - $op", $account)) {
    return TRUE;
  }
<?php endif; ?>

  $self = <?php echo $a->sMachineName(); ?>_entity_access_is_self($entity, $account);
  if ($self && user_access("own - $machine_name - $op", $account)) {
    return TRUE;
  }

<?php if($a->hasBundle()): ?>
  if ($self && $bundle !== NULL && user_access("own - $machine_name - $bundle - $op", $account)) {
    return TRUE;
  }
<?php endif; ?>

  return FALSE;
}


