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
function islandora_webform_user_can_submit_webform($node, $user = NULL, $pid = NULL) {

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


  // Check if this page is cached or not.
  $cached = $user->uid == 0 && (variable_get('cache', 0) || drupal_page_is_cacheable() === FALSE);

  // Check if the user can add another submission.
  if ($node->webform['submit_limit'] != -1) { // -1: Submissions are never throttled.
    module_load_include('inc', 'webform', 'includes/webform.submissions');
    if($pid && in_array($node->webform['submit_interval'], array(ISLANDORA_WEBFORM_SUBMIT_EVER_LIMIT_ID, ISLANDORA_WEBFORM_SUBMIT_ATATIME_LIMIT_ID))) {
      // Per user and object limit is in place.
      // See how many submissions for this user and compare to limit.
      $submission_count = islandora_webform_object_submission_count($node->nid, $pid, $node->webform['submit_interval'], $user->uid);
      if ($node->webform['submit_limit'] <= $submission_count) {
        $enabled = FALSE;
      }

    }
    // Disable the form if the limit is exceeded and page cache is not active.
    elseif (($user_limit_exceeded = webform_submission_user_limit_check($node)) && !$cached) {
      $enabled = FALSE;
    }
  }

  // Check if the user can add another submission if there is a limit on total
  // submissions.
  if ($node->webform['total_submit_limit'] != -1) { // -1: Submissions are never throttled.
    module_load_include('inc', 'webform', 'includes/webform.submissions');


    if($pid && in_array($node->webform['total_submit_interval'], array(ISLANDORA_WEBFORM_SUBMIT_EVER_LIMIT_ID, ISLANDORA_WEBFORM_SUBMIT_ATATIME_LIMIT_ID))) {
      // Total object limit is in place.
      $submission_count = islandora_webform_object_submission_count($node->nid, $pid, $node->webform['total_submit_interval']);
      if ($node->webform['total_submit_limit'] <= $submission_count) {
        $enabled = FALSE;
      }
    }
    // Disable the form if the limit is exceeded and page cache is not active.
    if (($total_limit_exceeded = webform_submission_total_limit_check($node)) && !$cached) {
      $enabled = FALSE;
    }
  }

  return $enabled;
}

function islandora_webform_object_submission_count($nid, $pid, $interval = NULL, $uid = NULL) {
  $query = db_select('webform_submissions', 's');
  $query->join('webform_component', 'c', "c.nid = s.nid and c.form_key = 'islandora_object_pid'");
  $query->join('webform_submitted_data', 'd', "d.sid = s.sid and d.cid = c.cid");
  $query->condition('s.nid', $nid);
  $query->condition('d.data', $pid);
  if ($uid) {
    $query->condition('s.uid', $uid);
  }
  $params = array(
    'nid' => $nid,
    'pid' => $pid,
    'interval' => $interval,
    'uid' => $uid,
  );
  drupal_alter('islandora_webform_object_submission_query_count', $query, $params);
  return $query->countQuery()->execute()->fetchField();
}

