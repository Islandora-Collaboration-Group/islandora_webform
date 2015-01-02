<?php

/**
 * Utility function to determine if user can access and submit this webform.
 *
 * This code is mostly lifted from the webform module's webform_node_view
 * function.
 *
 * @param object $node
 *   The webform node object
 * @param null $user
 *   A user object, or NULL
 *
 * @return bool
 *   If true, the user can submit; if false, the user cannot submit.
 *
 */
function islandora_webform_user_can_submit_webform($node, $user = NULL) {

  if (!$user) {
    global $user;
  }

  $page = node_is_page($node);
  $enabled = TRUE;
  $allowed_roles = array();

  // Check if the user's role can submit this webform.
  if (variable_get('webform_submission_access_control', 1)) {
    foreach ($node->webform['roles'] as $rid) {
      $allowed_roles[$rid] = isset($user->roles[$rid]) ? TRUE : FALSE;
    }
    if (array_search(TRUE, $allowed_roles) === FALSE && $user->uid != 1) {
      $enabled = FALSE;
    }
  }
  else {
    // If not using Webform submission access control, allow for all roles.
    $allowed_roles = array_keys(user_roles());
  }


  // Get a count of previous submissions by this user. Note that the
  // webform_submission_access() function may disable the page cache for
  // anonymous users if they are allowed to edit their own submissions!
  if ($page && webform_submission_access($node, NULL, 'list')) {
    module_load_include('inc', 'webform', 'includes/webform.submissions');
    $submission_count = webform_get_submission_count($node->nid, $user->uid);
  }

  // Check if this page is cached or not.
  $cached = $user->uid == 0 && (variable_get('cache', 0) || drupal_page_is_cacheable() === FALSE);

  // Check if the user can add another submission.
  if ($node->webform['submit_limit'] != -1) { // -1: Submissions are never throttled.
    module_load_include('inc', 'webform', 'includes/webform.submissions');

    // Disable the form if the limit is exceeded and page cache is not active.
    if (($user_limit_exceeded = webform_submission_user_limit_check($node)) && !$cached) {
      $enabled = FALSE;
    }
  }

  // Check if the user can add another submission if there is a limit on total
  // submissions.
  if ($node->webform['total_submit_limit'] != -1) { // -1: Submissions are never throttled.
    module_load_include('inc', 'webform', 'includes/webform.submissions');

    // Disable the form if the limit is exceeded and page cache is not active.
    if (($total_limit_exceeded = webform_submission_total_limit_check($node)) && !$cached) {
      $enabled = FALSE;
    }
  }

  return $enabled;
}
