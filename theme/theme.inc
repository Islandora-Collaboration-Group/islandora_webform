<?php

/**
 * Renders webform links for an object as appropriate.
 *
 * @param array $vars
 *   An array containing:
 *   - webform_data: Data from islandora_webform_webforms table.
 *   - islandora_object: The fedora object.
 *
 * @return string
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
    'html'       => TRUE,
    'query'      => array(
      'pid'         => $pid,
      $destination,
    ),
  );

  $tag_link_options = array(
    'attributes' => array(
      'class' => array(
        'tag-link',
        'use-ajax',
      ),
    ),
    'html'       => TRUE,
    'query'      => array(
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
    'html'       => TRUE,
    'query'      => array(
      $destination,
    ),
  );

  foreach ($webforms as $webform) {
    $jqid = 'iw_iwebform_links_' . $webform->entity_id;
    $output .= '<div id="' . $jqid . '" class="webform-links-wrapper"><div class="webform-links">';
    // Lets load the node.
    $webform_node_data = node_load($webform->entity_id)->webform;
    $inline = isset($webform_node_data['webform_ajax']) ? $webform_node_data['webform_ajax'] != WEBFORM_AJAX_NO_AJAX : FALSE;
    if ($inline) {
      $link_mode = 'inline';
      $webform_link_options['attributes']['class'] = array('webform-link', 'use-ajax');
      unset($webform_link_options['query']['destination']);
    }
    else {
      $link_mode = 'normal';
      $webform_link_options['attributes']['class'] = array('webform-link');
      $webform_link_options['attributes']['target'] = '_islandora_webform_page';
    }

    if ($webform->scope != 'tagged') {
      $output .= islandora_webform_theme_webform_link($webform, $webform_link_options, $link_mode);
    }
    else {
      // Determine if this webform is tagged to this object.
      module_load_include('inc', 'islandora_webform', 'includes/tag_object');
      $tagged = islandora_webform_is_webform_tagged($object, $webform->entity_id);

      // If so, add the webform link and the remove-tag link for content
      // managers.
      if ($tagged) {
        $output .= islandora_webform_theme_webform_link($webform, $webform_link_options, $link_mode);

        $path = 'islandora_webform/' . $webform->entity_id . '/untag_object/' . $pid . '/nojs';

        $untag_link_options['query']['tok'] = drupal_get_token('untag_link_' . $pid . '_' . $webform->entity_id);

        // drupal_valid_path() checks to see if the current user has access to
        // the add/remove tag links.
        if (drupal_valid_path($path, TRUE)) {
          $tag_link_options['query'] = $destination;
          $untag_link = l(t('<- remove this link'), $path, $untag_link_options);

          $output .= ' ' . $untag_link;
        }
      }
      else {
        // Otherwise, add the add-tag link for managers.
        $path = 'islandora_webform/' . $webform->entity_id . '/tag_object/' . $pid . '/nojs';

        // Add a token to the query.
        $tag_link_options['query']['tok'] = drupal_get_token('tag_link_' . $pid . '_' . $webform->entity_id);

        // Check access.
        if (drupal_valid_path($path, TRUE)) {
          $tag_link = l(t('add webform link: "%title"', array('%title' => $webform->link_text)), $path, $tag_link_options);

          $output .= $tag_link;
        }
      }
    }
    $output .= '</div></div>';
  }

  $output .= '</div>';

  return $output;
}

/**
 * Utility function to return a themed webform link for the current user.
 *
 * Return empty string if the user can't submit the webform
 *
 * @param object $islandora_webform_record
 *   one complete row from the islandora_webform_webforms table
 * @param array $webform_link_options
 *   an options array for the l() function. We expect it to have the "pid" query parameter
 *   included to make this a valid call to a islandora-enabled webform.
 * @param string $link_mode
 *   If value is 'inline', then an ajax link is created.
 *
 * @return string
 *   The themed link, or empty if no access
 */
function islandora_webform_theme_webform_link($islandora_webform_record, $webform_link_options = array(), $link_mode = 'inline') {
  $webform_link = '';
  $node = node_load($islandora_webform_record->entity_id);
  module_load_include('inc', 'islandora_webform', 'includes/webform_access');
  if (islandora_webform_user_can_submit_webform($node)) {
    $link_path = $link_mode == 'inline' ? 'islandora_webform/retrieve_form/' . $islandora_webform_record->entity_id . '/nojs' : drupal_get_path_alias('node/' . $islandora_webform_record->entity_id);
    $webform_link_options['attributes']['title'] = $islandora_webform_record->link_help;
    $webform_link = l($islandora_webform_record->link_text, $link_path, $webform_link_options);
  }
  return $webform_link;
}
