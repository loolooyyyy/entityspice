<?php print $a['php']; ?>

function <?php echo $a['safe'] . $a['machine_name'] ?>_entity_access_is_self($entity, $account) {
  if ($entity == NULL) {
    return FALSE;
  }

  // HANDLE SELF_ENTITY LOGIC HERE
  //  if($has_uid) {
  //
  //  }
  return FALSE;
}


function <?php echo $a['safe'] . $a['machine_name'] ?>_entity_access($op,<?php if($a['has_bundle']): ?> $bundle = NULL,<?php endif; ?> $entity = NULL, $user = NULL) {
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

<?php if($a['has_bundle']): ?>
  if ($bundle !== NULL && user_access("$machine_name - $bundle - $op", $account)) {
    return TRUE;
  }
<?php endif; ?>

  $self = <?php echo $a['safe'] . $a['machine_name'] ?>_entity_access_is_self($entity, $account);
  if ($self && user_access("own - $machine_name - $op", $account)) {
    return TRUE;
  }

<?php if($a['has_bundle']): ?>
  if ($self && $bundle !== NULL && user_access("own - $machine_name - $bundle - $op", $account)) {
    return TRUE;
  }
<?php endif; ?>

  return FALSE;
}


