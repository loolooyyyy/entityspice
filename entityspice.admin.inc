<?php

/**
 * Given a field types, List all fields available of that field type.
 *
 * Good for when you want user to select an available field of specific field
 * type in form.
 *
 * @param string $field_type
 *   Machine name of requested field type.
 * @param boolean $has_none_option
 *   Whether to add a 'NONE' option as the list's first element, good for a list
 *   with optional options.
 *
 * @return array suitable for form API select element.
 */
function _entityspice_generate_field_list($field_type, $has_none_option) {
  // Also correctly checks an empty array.
  $fields = $has_none_option ? ['NONE' => t('NONE')] : [];
  foreach (field_info_fields() as $field) {
    if ($field_type && $field['type'] != $field_type) {
      continue;
    }
    $fields[$field['field_name']] = $field['field_name'];
  }
  sort($fields);
  return $fields;
}

/**
 * Same as _entityspice_generate_field_list but only fields belonging to a
 * bundle of specific entity type.
 *
 * @param $etype string entity type
 * @param $bundle string entity bundle
 * @param bool $none_option whether to include a 'none' option in the list.
 *
 * @return array list of fields.
 */
function _entityspice_generate_field_list_from_bundle($etype, $bundle, $none_option) {
  $fields = $none_option ? ['NONE' => t('NONE')] : [];
  foreach (field_info_instances($etype, $bundle) as $name => $crap) {
    $fields[$name] = check_plain($name) . ' - ' . check_plain($crap['label']);
  }
  ksort($fields);
  return $fields;
}
