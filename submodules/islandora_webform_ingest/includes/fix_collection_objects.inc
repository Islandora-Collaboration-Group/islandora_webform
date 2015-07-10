<?php

/**
 * Fix collection relationships.
 *
 * Search for all objects that have rels-ext relations to collections, and
 * re-set their relationship predicate to 'isMemberOfCollection'.
 */
function iwi_retroactive_fix_collection_predicates() {

  // Get list of available relationship predicates.
  $predicates = islandora_webform_ingest_rels_ext_predicates_options();
  unset($predicates['isMemberOfCollection']);

  // Find all collection objects.
  $collections = iwi_get_all_collection_objects();

  // Iterate through each collection object, and find objects related to
  // it through each available relationship predicate in turn.
  foreach ($collections as $collection) {
    $pid = $collection['pid'];

    foreach (array_keys($predicates) as $predicate) {
      // Create sparql query to find child objects related to this collection
      // with this predicate.
      $query = <<<EOQ
SELECT ?pid ?label
FROM <#ri>
WHERE {
  ?pid <fedora-model:label> ?label ;
       <fedora-model:$predicate> <info:fedora/$pid>
}
EOQ;
      $connection = islandora_get_tuque_connection();
      $child_objects = $connection->repository->ri->sparqlQuery($query);

      // Loop through the child objects, removing the old relationship and
      // adding the isMemberOfCollection relationship.
      if (!empty($child_objects)) {
        foreach ($child_objects as $target_object) {
          $target_object->relationships->remove(FEDORA_RELS_EXT_URI, $predicate);

          // Check to see if there already is an isMemberOfCollection relation;
          // Add if not present.
          $memberofcollection_relationships = $target_object->relationships->get(FEDORA_RELS_EXT_URI, 'isMemberOfCollection');
          if (empty($memberofcollection_relationships)) {
            $target_object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', $collection, RELS_TYPE_URI);
          }
        }
      }
    }
  }
}

/**
 * Retroactively add/replace IW ingested object tracking tags.
 */
function iwi_retroactive_fix_iw_relation_tags() {

  // Get PIDs and webform nids from recorded ingestions.
  $query = db_select('islandora_webform_ingestions', 'i')->distinct()->fields('i', array('nid', 'pid'));
  $results = $query->execute()->fetchAll();
  foreach ($results as $result) {
    $object = islandora_object_load($result->pid);

    // Add our control predicates, so we can keep track of this object.
    $object->relationships->remove(FEDORA_RELS_EXT_URI, ISLANDORA_WEBFORM_ISIW_PREDICATE);
    $object->relationships->remove(FEDORA_RELS_EXT_URI, ISLANDORA_WEBFORM_HASIW_PREDICATE);
    $object->relationships->add(ISLANDORA_RELS_EXT_URI, ISLANDORA_WEBFORM_ISIW_PREDICATE, "true", RELS_TYPE_PLAIN_LITERAL);
    $object->relationships->add(ISLANDORA_RELS_EXT_URI, ISLANDORA_WEBFORM_HASIW_PREDICATE, "{$result->nid}", RELS_TYPE_PLAIN_LITERAL);

  }
}


/**
 * Get all islandora collections as objects.
 *
 * @return mixed
 *
 */
function iwi_get_all_collection_objects() {
  $content_model = 'islandora:collectionCModel';
  $query = <<<EOQ
SELECT ?pid ?label
FROM <#ri>
WHERE {
  ?pid <fedora-model:label> ?label ;
       <fedora-model:hasModel> <info:fedora/$content_model>
}
EOQ;
  // Connect to Tuque and grab the results.
  $connection = islandora_get_tuque_connection();
  return $connection->repository->ri->sparqlQuery($query);
}