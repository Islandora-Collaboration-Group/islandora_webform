<?php

/**
 * Implementation of preprocess hook for all solution pack object pages
 *
 * @param $variables
 */
function islandora_webform_preprocess_solution_pack_module(&$variables) {
  drupal_add_library('system', 'drupal.ajax');

  if (!empty($variables['islandora_object']->id)) {
    $pid = $variables['islandora_object']->id;
    $sql = 'SELECT w.*, n.title as title FROM {islandora_webform_webforms} w JOIN {node} n ON (n.nid = w.entity_id) WHERE w.enabled = :enabled AND (:pid REGEXP w.pid_filter OR w.pid_filter = \'\')';
    $result = db_query($sql, array(':enabled' => 1, ':pid' => $pid));
    $variables['webforms'] = $result->fetchAll();
    // TODO Get webform tags existing in object datastreams
    if (!empty($variables['webforms'])) {

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
      $tag_link_options = array(
        'attributes' => array(
          'class' => array(
            'tag-link'
          ),
        ),
        'html' => TRUE,
      );
      $untag_link_options = array(
        'attributes' => array(
          'class' => array(
            'untag-link'
          ),
        ),
        'html' => TRUE,
      );

      foreach ($variables['webforms'] as $webform) {

        $output .= '<div class="webform-links">';

        $webform_link_options['attributes']['title'] = $webform->link_help;
        $webform_link = l($webform->link_text, drupal_get_path_alias('node/' . $webform->entity_id), $webform_link_options);

        if ($webform->scope != 'tagged') {
          $output .= $webform_link;
        }
        else {
          // Determine if this webform is tagged to this object.
          $tagged = db_select('islandora_webform_object_tags', 't')
            ->condition('pid', $pid)
            ->condition('entity_id', $webform->entity_id)
            ->countQuery()->execute()->fetchField();

          // If so, add the webform link and the remove-tag link for content managers.
          if ($tagged) {
            $output .= $webform_link;
            $path = 'islandora_webform/' . $webform->entity_id . '/untag_object/' . $pid;

            // This checks to see if the current user has access to the add/remove tag links
            if (drupal_valid_path($path, TRUE)) {
              $tag_link_options['query'] = $destination;
              $untag_link = l(t('<- remove this link'), $path, $untag_link_options);

              $output .= ' ' . $untag_link;
            }
          }
          else {
            // Otherwise, add the add-tag link for managers.
            $path = 'islandora_webform/' . $webform->entity_id . '/tag_object/' . $pid;

            // Check access
            if (drupal_valid_path($path, TRUE)) {
              $tag_link_options['query'] = $destination;
              $tag_link = l(t('add webform link "%title"', array('%title' => $webform->link_text)), $path, $tag_link_options);

              $output .= $tag_link;
            }
          }
        }
        $output .= '</div>';
      }
      $output .= '</div>';

      $variables['metadata'] .= $output;
    }
  }
}
