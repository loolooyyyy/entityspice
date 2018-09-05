<?php echo $a->php; ?>

/**
 * Implements hook_theme().
 *
 *   - Defines theme callback for 'add entity ...' page.
 *   - Defines theme callback for default page.
 *   - Defines theme callback for property field of entity (pseudo fields).
 */
function <?php echo $a->m; ?>_theme() {
  $info = [
    '<?php echo $a->m; ?>_entity_add_list' => ['content', 'entity_type'],
    '<?php echo $a->m; ?>_entity_list' => ['header', 'rows', 'entity_type', 'entities'],
    '<?php echo $a->m; ?>_default_page' => ['entity_types'],
  ];

  $theme = [];
  foreach ($info as $t_name => $t_vars) {
    foreach ($t_vars as $var) {
      $theme[$t_name]['variables'][$var] = NULL;
    }
  }

  $theme['<?php echo $a->m; ?>_entity_property_field'] = [
    'variables' => [
      'label_hidden' => FALSE,
      'title_attributes' => NULL,
      'label' => '',
      'content_attributes' => NULL,
      'items' => [],
      'item_attributes' => [0 => ''],
      'classes' => '',
      'attributes' => '',
    ],
  ];

  return $theme;
}

/**
 * Returns HTML for a list of available node types for node creation.
 *
 * @param array $variables
 *   An associative array containing:
 *   - content: An array of content types.
 *
 * @ingroup themeable
 * @return string
 */
function theme_<?php echo $a->m; ?>_entity_add_list($variables) {
  $content = $variables['content'];
  $etype = $variables['entity_type'];
  $bundles_page = '<?php echo $a->parent_admin_path; ?>';

  if ($content) {
    $output = '<dl class="node-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l($item['title'], $item['href'], $item['localized_options']) . '</dt>';
      $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
    }
    $output .= '</dl>';
  }
  else {
    $args = [
      '@create-bundle' => url($bundles_page),
      '@etype' => $variables['entity_type'],
    ];
    $output = '<p>';
    $output .= t('You have not created any @etype yet. Go to the <a href="@create-bundle"> bundle administration page</a> to add a new @etype bundle.', $args);
    $output .= '</p>';
  }

  return $output;
}

/**
 * Theme function for entity list.
 */
function theme_<?php echo $a->m; ?>_entity_list($variables) {
  $header = $variables['header'];
  $rows = $variables['rows'];
  $etype = $variables['entity_type'];

  $render['table'] = [
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No entities available.'),
  ];
  $render['pager'] = ['#theme' => 'pager'];

  return render($render);
}

/**
 * Returns HTML for a list of available entity types for entity creation.
 *
 * @param array $variables
 *   An associative array containing:
 *   - entity_types: An array of entity types.
 *
 * @ingroup themeable
 * @return string
 */
function theme_<?php echo $a->m; ?>_default_page($variables) {
  $content = $variables['content'];

  if ($content) {
    $output = '<dl class="node-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l($item['title'], $item['href'], $item['localized_options']) . '</dt>';
      $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
    }
    $output .= '</dl>';
  }
  else {
    $output = '<p>';
    $output .= t('You have not created any ??? yet.');
    $output .= '</p>';
  }

  return $output;
}

/**
 * Theme function for entity properties.
 *
 * Simple wrapper around theme_field that sets default values and ensures
 * properties render consistently with fields.
 */
function theme_<?php echo $a->m; ?>_entity_property_field($variables) {
  return theme('field', $variables);
}
