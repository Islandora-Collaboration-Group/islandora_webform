<?php

/**
 * @file
 * Provides islandora webform tagging callback functions and utilities.
 *
 */

define('ISLANDORA_WEBFORM_WEBFORM_LINK_DATASTREAM_ID', 'WF');


/**
 * Callback to add a webform link to an object's metadata.
 *
 * @int $webform_id
 *  The node id of the webform
 *
 * @string $pid
 *  The fedora object id
 */
function islandora_webform_action_tag_object($webform_id, $pid) {
  islandora_webform_webform_link_update($pid, $webform_id, 'add');
  drupal_goto();
}

/**
 * Callback to remove a webform link from an object's metadata.
 *
 * @int $webform_id
 *  The node id of the webform
 *
 * @string $pid
 *  The fedora object id
 */
function islandora_webform_action_untag_object($webform, $pid) {
  islandora_webform_webform_link_update($pid, $webform, 'delete');
  drupal_goto();
}

/**
 * Store/remove webform ids in WF datastream
 *
 * This datastream is just a plain text mimetype, with tokenized (space-separated)
 * node ids for the webforms, which can be indexed and searched by SOLR.
 *
 * Create the datastream if needed; delete it if no longer needed.
 *
 * @string $pid
 *  a fedora object pid
 *
 * @int $webform_id
 *  the node id of the webform that we are linking to this object
 *
 * @string $op
 *  operation: 'add' or 'delete' - indicates whether we are adding the link or removing it
 */
function islandora_webform_webform_link_update($pid, $webform_id, $op = 'add') {

  $object = islandora_object_load($pid);
  if ($object) {

    // Check if the datastream exists on this object
    $datastream = islandora_datastream_load(ISLANDORA_WEBFORM_WEBFORM_LINK_DATASTREAM_ID, $object);

    // Tokenize webform nid
    $webform_token = _islandora_webform_tokenize_webform_id($webform_id);

    // If not preexisting, create the datastream and add the webform nid
    // as its content.
    if (!$datastream) {
      if ($op == 'add') {
        $datastream = $object->constructDatastream(ISLANDORA_WEBFORM_WEBFORM_LINK_DATASTREAM_ID);
        $datastream->label = 'Webform Links: A list of webform node ids associated with this object';
        $datastream->mimeType = 'text/plain';
        $datastream->setContentFromString($webform_token);
        $object->ingestDatastream($datastream);
      }
    }

    // If preexisting, look to see if the webform nid is already there
    else {
      $contents = $datastream->content;

      // If $webform_token doesn't already exist and we're adding, then append it
      if (strpos($contents, $webform_token) === FALSE) {

        if ($op = 'add') {
          $contents .= $webform_token;
          $datastream->setContentFromString($contents);
          $object->ingestDatastream($datastream);
        }

      }
      else {
        // $webform_token exists, if not adding, then remove it from the
        // content string.
        if ($op != 'add') {
          $contents = str_replace($webform_token, '', $contents);

          // Update or delete the datastream, depending on presence of content.
          if (strlen($contents) > 0) {

            // We have content, so update the datastream.
            $datastream->setContentFromString($contents);
            $object->ingestDatastream($datastream);
          }
          else {

            // No content: datastream is not needed, so delete it.
            islandora_delete_datastream($datastream);
          }
        }
      }
    }
  }
}

/**
 * Utility function wraps webform nid in spaces so that we can't accidentally
 * search and find portion of the string
 *
 * @int $webform_nid
 *
 * @return string
 */
function _islandora_webform_tokenize_webform_id($webform_id) {

  return ' ' . $webform_id . ' ';

}


/**
 * Figure out if a webform is linked to an object by looking at the string
 * stored in the WF datastream to see if it contains the nid of this webform.
 *
 * @param $object
 *  a fedora object
 *
 * @param $webform_id
 *  a webform node id
 *
 * @return bool
 *  TRUE if this object has been tagged for this webform
 *  otherwise FALSE
 */
function islandora_webform_is_webform_tagged($object, $webform_id) {

  $webform_token = _islandora_webform_tokenize_webform_id($webform_id);

  // Try to load the datastream
  $datastream = islandora_datastream_load(ISLANDORA_WEBFORM_WEBFORM_LINK_DATASTREAM_ID, $object);

  // It's tagged if the datastream exists
  // and if the webform nid is in its content string.
  return !empty($datastream) && (strpos($datastream->content, $webform_token) !== FALSE);

}