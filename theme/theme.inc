<?php

/**
 * Renders webform links for an object as appropriate.
 *
 * @param array $vars
 *   An array containing:
 *   - webform_data: Data from islandora_webform_webforms table.
 *   - islandora_object: The fedora object.
 *
 * @return string $output
 *   Themed links
 */
function theme_islandora_webform_links(array $vars) {
  drupal_add_library('system', 'drupal.ajax');
  drupal_add_css(drupal_get_path('module', 'islandora_webform') . '/css/islandora_webform.css');

  $webforms = $vars['webform_data'];
  $object = $vars['islandora_object'];
  $pid = $object->id;
  module_load_include('inc', 'islandora_webform', 'includes/utilities');

  $output = '<div id="islandora_webform_object_tag_links">';

  $destination = drupal_get_destination();

  $webform_link_options = array(
    'query' => array(
      'pid' => $pid,
      $destination,
    ),
    'class' => array(
      'webform-link'
    ),
  );

  if(variable_get('islandora_webform_link_target_new_page', 0)) {
    $webform_link_options['attributes']['target'] = '_islandora_webform_page';
  }

  $tag_link_options = array(
    'attributes' => array(
      'class' => array(
        'tag-link',
        'use-ajax',
      ),
    ),
    'html' => TRUE,
    'query' => array(
      $destination,

    ),
  );
  $untag_link_options = array(
    'attributes' => array(
      'class' => array(
        'untag-link',
        'use-ajax',
      ),
    ),
    'html' => TRUE,
    'query' => array(
      $destination
    ),
  );


  $output .= '<div class="webform-links">';

  foreach ($webforms as $webform) {

    $webform_link_options['attributes']['title'] = $webform->link_help;
    $webform_link = l($webform->link_text, drupal_get_path_alias('node/' . $webform->entity_id), $webform_link_options);

    if ($webform->scope != 'tagged') {
      $output .= $webform_link;
    }
    else {
      // Determine if this webform is tagged to this object.
      module_load_include('inc', 'islandora_webform', 'includes/tag_object');
      $tagged = islandora_webform_is_webform_tagged($object, $webform->entity_id);

      // If so, add the webform link and the remove-tag link for content managers.
      if ($tagged) {
        $output .= $webform_link;
        $path = 'islandora_webform/' . $webform->entity_id . '/untag_object/' . $pid . '/nojs';

        $untag_link_options['query']['tok'] = drupal_get_token('untag_link_' . $pid . '_' . $webform->entity_id);

        // drupal_valid_path() checks to see if the current user has access to the add/remove tag links
        if (drupal_valid_path($path, TRUE)) {
          $tag_link_options['query'] = $destination;
          $untag_link = l(t('<- remove this link'), $path, $untag_link_options);

          $output .= ' ' . $untag_link;
        }
      }
      else {
        // Otherwise, add the add-tag link for managers.
        $path = 'islandora_webform/' . $webform->entity_id . '/tag_object/' . $pid . '/nojs';

        // Add a token to the query to prevent
        $tag_link_options['query']['tok'] = drupal_get_token('tag_link_' . $pid . '_' . $webform->entity_id);

        // Check access
        if (drupal_valid_path($path, TRUE)) {
          $tag_link = l(t('add webform link: "%title"', array('%title' => $webform->link_text)), $path, $tag_link_options);

          $output .= $tag_link;
        }
      }
    }
  }
  $output .= '</div>';

  return $output;
}


/**
 * Implementation of preprocess hook for all solution pack object pages
 *
 * @param $variables
 */
function islandora_webform_preprocess_solution_pack_module(&$variables) {

  if (!empty($variables['islandora_object'])) {
    module_load_include('inc', 'islandora_webform', 'includes/utilities');
    $variables['webforms'] = islandora_webform_object_get_webforms($variables['islandora_object']);

    if (!empty($variables['webforms'])) {

      $output = theme('islandora_webform_links', array(
        'webform_data' => $variables['webforms'],
        'islandora_object' => $variables['islandora_object'],
      ));

      // Add to the template variables.
      $variables['webform_links'] = $output;

      // If webform link injection is not disabled, append to the
      // metadata table.
      if(variable_get('islandora_webform_inject_link', 1)) {
        $variables['metadata'] .= $output;

      }
    }
  }
}
