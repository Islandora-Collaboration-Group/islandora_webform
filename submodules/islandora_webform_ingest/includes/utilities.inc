<?php

/**
 * Get a list of datastream ids that could be ingested to
 *
 * @param IslandoraFedoraObject $object
 * @return array $results
 *  An array of the ingestable datastream objects
 */
function iwi_ingestable_datastreams(IslandoraFedoraObject $object, $mimetype = NULL) {
  $results = array();
  foreach ($object as $datastream) {
    if (! $mimetype || strpos($datastream->mimetype, $mimetype) !== FALSE) {
      $results[] = $datastream;
    }
  }
  return $results;
}

function iwi_datastream_fields_list(array $datastreams, string $mimetype) {
  foreach($datastreams as $datastream) {

  }
}