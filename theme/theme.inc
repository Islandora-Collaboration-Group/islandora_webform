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
    'html' => TRUE,
    'query' => array(
      'pid' => $pid,
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
      $destination,
    ),
  );

  $link_mode = variable_get('islandora_webform_link_target', 'new');
  foreach ($webforms as $webform) {
    $jqid = 'iw_iwebform_links_' . $webform->entity_id;
    $output .= '<div id="' . $jqid . '" class="webform-links-wrapper"><div class="webform-links">';
    // Lets load the node.
    $swebform_node = node_load($webform->entity_id);
    $webform_node_data = $swebform_node->webform;

    $inline = module_exists('webform_ajax') && isset($webform_node_data['webform_ajax']) ? $webform_node_data['webform_ajax'] != WEBFORM_AJAX_NO_AJAX : FALSE;
    if ($inline) {
      $link_mode = 'inline';
      $webform_link_options['attributes']['class'] = array(
        'webform-link',
        'use-ajax'
      );
      unset($webform_link_options['query']['destination']);
    }
    else {
      $webform_link_options['attributes']['class'] = array('webform-link');
      if ($link_mode != 'same') {
        $webform_link_options['attributes']['target'] = '_islandora_webform_page';
      }
    }

    // Show previous submissions by this user if they can be edited.
    global $user;
    module_load_include('inc', 'webform', 'includes/webform.submissions');
    if (!webform_submission_user_limit_check($swebform_node)
      && !webform_submission_total_limit_check($swebform_node)
    ) {

      // Get a list of submission ids that this user has created for this object
      // and this webform.
      $query = db_select('webform_submissions', 's')
        ->condition('s.nid', $swebform_node->nid)
        ->fields('s', array('sid'));
      if ($user->uid) {
        $query->condition('uid', $user->uid);
      }
      else {
        $query->condition('remote_addr', ip_address());
      }
      // Join the submitted data table to look for the PID of this object.
      $query->join('webform_submitted_data', 'd', "d.sid = s.sid");
      $query->condition('d.data', $pid);
      $sids = $query->execute()->fetchCol();

      $submissions = webform_get_submissions(array(
        'nid' => $swebform_node->nid,
        'sid' => $sids
      ));

      // Check if this user has edit access to this submission. If not, remove it.
      if (!empty($submissions)) {
        module_load_include('inc', 'islandora_webform', 'includes/inline_webform');
        foreach ($submissions as $key => $submission) {
          if (!webform_submission_access($swebform_node, $submission, 'edit', $user)) {
            unset($submissions[$key]);
          }
        }
      }

      // If the number of previous submissions is more than one, display links to
      // edit the submissions.
      if (count($submissions) > 1) {
        foreach ($submissions as $editable_submission) {
          $output .= islandora_webform_theme_webform_link($webform, $webform_link_options, $pid, $link_mode, $editable_submission->sid);
        }
      }
      // If instead the number of editable submissions is exactly one, don't show links.
      // Rather, display the form inline.
      elseif(!empty($submissions)) {
        $output = _iw_inline_webform($swebform_node, 'nojs', reset($submissions));
      }
    }

    // If this object is not tagged for this webform but can be, show a link for that.
    if ($webform->scope != 'tagged') {
      $output .= islandora_webform_theme_webform_link($webform, $webform_link_options, $pid, $link_mode);
    }
    else {
      // Determine if this webform is tagged to this object.
      module_load_include('inc', 'islandora_webform', 'includes/tag_object');
      $tagged = islandora_webform_is_webform_tagged($object, $webform->entity_id);

      // If so, add the webform link and the remove-tag link for content
      // managers.
      if ($tagged) {
        $output .= islandora_webform_theme_webform_link($webform, $webform_link_options, $pid, $link_mode);

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
 * @param array  $webform_link_options
 *   an options array for the l() function. We expect it to have the "pid" query parameter
 *   included to make this a valid call to a islandora-enabled webform.
 * @param string $link_mode
 *   If value is 'inline', then an ajax link is created.
 * @param string $sid
 *   If the link is to edit a previous submission, this is that submission's sid
 *
 * @return string
 *   The themed link, or empty if no access
 */
function islandora_webform_theme_webform_link($islandora_webform_record, $webform_link_options = array(), $pid, $link_mode = 'inline', $sid = NULL) {
  $webform_link = '';
  $islandora_webform_record->link_mode = $link_mode;
  $node = node_load($islandora_webform_record->entity_id);
  module_load_include('inc', 'islandora_webform', 'includes/webform_access');
  if ($node->status && islandora_webform_user_can_submit_webform($node, NULL, $pid)) {
    $link_path = $link_mode == 'inline' ? 'islandora_webform/retrieve_form/' . $islandora_webform_record->entity_id . '/nojs' : drupal_get_path_alias('node/' . $islandora_webform_record->entity_id);
    $link_text = $islandora_webform_record->link_text;
    if ($sid) {
      $submission = webform_get_submission($node->nid, $sid);

      $link_path .= '/submission/' . $sid . '/edit';
      $when = max($submission->submitted, $submission->completed, $submission->modified);
      $link_text = t('!type, last worked on !when by !who', array(
        '!type' => $submission->completed ? 'Submission' : 'Draft',
        '!when' => format_date($when),
        '!who' => $submission->name,
      ));
      $islandora_webform_record->link_help = t('Click to modify or add to this previous !type.',
        array(
          '!type' => $submission->completed ? 'submission' : 'draft',
        ));
      $webform_link_options['attributes']['class'][] = $submission->completed ? 'submission' : 'draft';
    }
    $webform_link_options['attributes']['title'] = $islandora_webform_record->link_help;
    $webform_link = l($link_text, $link_path, $webform_link_options);
  }
  return $webform_link;
}
