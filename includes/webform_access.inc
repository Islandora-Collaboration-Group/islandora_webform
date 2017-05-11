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
 * @return mixed bool|array
 *   Array of draft submission for the user, if present, otherwise boolean:
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

  if($node->webform['allow_draft']) {

  }

  // Check if the user can add another submission.
  if ($node->webform['submit_limit'] != -1) { // -1: Submissions are never throttled.
    module_load_include('inc', 'webform', 'includes/webform.submissions');
      if($pid && $node->webform['submit_interval'] == ISLANDORA_WEBFORM_SUBMIT_PER_OBJECT_LIMIT_ID) {
      // Per user and object limit is in place.
      // See how many submissions for this user and compare to limit.
      $ws_limits = @unserialize($node->webform['user_ws_limits']);
      $user_submissions = islandora_webform_object_submission_count($node->nid, $pid, $node->webform['submit_interval'], $user->uid, $ws_limits);
        $draft_user_submissions = array_filter($user_submissions['results'], function($r) {return $r->is_draft == 1;});
        $draft_user_submissions_count = count($draft_user_submissions);
      if (!$draft_user_submissions_count && $node->webform['submit_limit'] <= $user_submissions['count']) {
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
  if (!$draft_user_submissions_count && $node->webform['total_submit_limit'] != -1) { // -1: Submissions are never throttled.
    module_load_include('inc', 'webform', 'includes/webform.submissions');


    if($pid && $node->webform['total_submit_interval'] == ISLANDORA_WEBFORM_SUBMIT_PER_OBJECT_LIMIT_ID) {
      // Total object limit is in place.
      $ws_limits = @unserialize($node->webform['total_ws_limits']);
      $submission_count = islandora_webform_object_submission_count($node->nid, $pid, $node->webform['total_submit_interval'], NULL, $ws_limits);
      if ($node->webform['total_submit_limit'] <= $submission_count['count']) {
        $enabled = FALSE;
      }
    }
    // Disable the form if the limit is exceeded and page cache is not active.
    if (($total_limit_exceeded = webform_submission_total_limit_check($node)) && !$cached) {
      $enabled = FALSE;
    }
  }
  if(!empty($draft_user_submissions)) {
    return $draft_user_submissions;
  }
  else {
    return $enabled;
  }
}

/**
 * Count the number of submissions that meet the limit criteria.
 *
 * @param       $nid
 * @param       $pid
 * @param null  $interval
 * @param null  $uid
 * @param array $ws_filter
 *
 * @return mixed
 */
function islandora_webform_object_submission_count($nid, $pid, $interval = NULL, $uid = NULL, $ws_filter = array()) {
  $query = db_select('webform_submissions', 's');
  $query->fields('s', array('sid', 'is_draft'));
  $query->distinct();
  $query->join('webform_component', 'c', "c.nid = s.nid and c.form_key = 'islandora_object_pid'");
  $query->join('webform_submitted_data', 'd', "d.sid = s.sid and d.cid = c.cid");
  $query->condition('s.nid', $nid);
  $query->condition('d.data', $pid);
  if ($uid) {
    $query->condition('s.uid', $uid);
  }
  if (!empty($ws_filter)) {
    $ws_conditions = db_or();
    // Separate out draft, completed and ingested from webform workflow states filter.
    $core_states = array();
    $core_states['draft'] = array_search('draft', $ws_filter);
    $core_states['completed'] = array_search('completed', $ws_filter);
    $core_states['ingested'] = array_search('ingested', $ws_filter);
    $core_states['not-ingested'] = array_search('not-ingested', $ws_filter);
    $core_states = array_filter($core_states);
    foreach($core_states as $id => $core_state) {
      switch($id) {
        case 'draft':
          $ws_conditions->condition('s.is_draft', 1);
          break;
        case 'completed':
          $ws_conditions->condition('s.is_draft', 0);
          break;
        case 'ingested':
          $query->leftJoin('islandora_webform_ingestions', 'iwi', 'iwi.sid = s.sid AND iwi.nid = s.nid');
          $ws_conditions->isNotNull('iwi.sid');
          $iwi_table_joined = TRUE;
          break;
        case 'not-ingested':
          $current_tables = $query->getTables();
          if(!empty($current_tables['iwi'])) {
            $query->leftJoin('islandora_webform_ingestions', 'iwi', 'iwi.sid = s.sid AND iwi.nid = s.nid');
          }
          $ws_conditions->isNotNull('iwi.sid');
          break;
      }
      unset($ws_filter[$id]);
    }

    // Handle rwemaining webform workflow state filters.
    if (!empty($ws_filter)) {
      $query->leftJoin('webform_workflow_submissions', 'ws', "ws.sid = s.sid");
      $ws_conditions->condition('ws.wsid', $ws_filter);
    }
    $query->condition($ws_conditions);
  }

  // Permit modules to add additional logic to the submissions count query.
  $params = array(
    'nid' => $nid,
    'pid' => $pid,
    'interval' => $interval,
    'uid' => $uid,
  );
  drupal_alter('islandora_webform_object_submission_query_count', $query, $params);

  $results = $query->execute()->fetchAll();
  return array('count' => count($results), 'results' => $results);
}

