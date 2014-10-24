<?php

/**
 * @file
 * Provides islandora webform tagging callback functions
 *
 */

function islandora_webform_action_tag_object($webform, $pid) {
  db_insert('islandora_webform_object_tags')
    ->fields(array(
        'entity_id' => $webform,
        'pid' => $pid,
      )
    )
    ->execute();
  drupal_goto();
}

function islandora_webform_action_untag_object($webform, $pid) {
  db_delete('islandora_webform_object_tags')
    ->condition('entity_id', $webform)
    ->condition('pid', $pid)
    ->execute();
  drupal_goto();
}

