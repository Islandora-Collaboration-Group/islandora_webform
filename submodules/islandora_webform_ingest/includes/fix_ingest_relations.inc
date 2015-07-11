<?php

/**
 * Retroactive IWI fix page.
 */
function islandora_ingest_webform_admin_settings() {
  $form = array(
    'retroactive_fix' => array(
      '#type' => 'fieldset',
      '#title' => 'Repair relations',
      '#weight' => 5,
      '#collapsible' => FALSE,
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Go'),
        '#submit' => array('iwi_retroactive_fix_collection_submit'),
        '#prefix' => '<p>' . t('Repair collection object predicates and add/replace islandora webform relation tags') . '</p>',
      ),
    ),
  );

  return $form;
}

/**
 * Run retroactive relationship fixes.
 */
function iwi_retroactive_fix_collection_submit() {
  iwi_retroactive_fix_all_collection_predicates();
  iwi_retroactive_fix_all_iw_relation_tags();

  batch_process();
}

/**
 * Fix collection relationships.
 *
 * Search for all objects that have rels-ext relations to collections, and
 * re-set their relationship predicate to 'isMemberOfCollection'.
 */
function iwi_retroactive_fix_all_collection_predicates() {

  module_load_include('inc', 'islandora', 'includes/utilities');

  // Get list of available relationship predicates.
  $predicates = islandora_webform_ingest_rels_ext_predicates_options();
  unset($predicates['isMemberOfCollection']);

  // Find all collection objects.
  $collections = iwi_get_all_collection_objects();

  // Iterate through each collection object, and find objects related to
  // it through each available relationship predicate in turn.
  $operations = array();

  foreach ($collections as $collection) {
    $pid = $collection['pid'];

    foreach (array_keys($predicates) as $predicate) {
      array_push($operations, array(
          "iwi_fix_retroactive_single_collection_predicate",
          array($pid, $predicate),
        )
      );
    }
  }
  // Construct the batch array for processing.
  $batch = array(
    'operations' => $operations,
    'title' => t('Fix collection predicates'),
    'finished' => 'iwi_retroactive_fix_all_collection_predicates_finished',
    'file' => drupal_get_path('module', 'islandora_webform_ingest') . '/includes/fix_ingest_relations.inc',
  );

  batch_set($batch);
}

/**
 * Repair collection predicate for a single collection member.
 *
 * @param int $pid
 *   The pid of the object that is a member of a collection.
 * @param string $predicate
 *   The predicate that we want to change to isMemberOfCollection
 */
function iwi_fix_retroactive_single_collection_predicate($pid, $predicate) {
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

  $collection = islandora_object_load($pid);

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

/**
 * Finished batch callback for collection predicates repair.
 */
function iwi_retroactive_fix_all_collection_predicates_finished($success, $results, $operations) {
  if ($success) {
    $message = format_plural(count($results), 'One collection predicate repaired.', '@count collection predicates repaired.');
  }
  else {
    $message = t('Finished with an error.');
  }
  drupal_set_message($message);
}

/**
 * Retroactively add/replace IW ingested object tracking tags.
 */
function iwi_retroactive_fix_all_iw_relation_tags() {
  // Get PIDs and webform nids from recorded ingestions.
  $query = db_select('islandora_webform_ingestions', 'i')
    ->distinct()
    ->fields('i', array('nid', 'pid'));
  $ingestions = $query->execute()->fetchAll();

  $operations = array();

  foreach ($ingestions as $ingestion) {
    array_push($operations, array(
        "iwi_retroactive_fix_single_iw_relation_tag",
        array($ingestion->pid, $ingestion->nid),
      )
    );
  }
  // Construct the batch array for processing.
  $batch = array(
    'operations' => $operations,
    'title' => t('Add/replace IW relation tags'),
    'finished' => 'iwi_retroactive_fix_all_iw_relation_tags_finished',
    'file' => drupal_get_path('module', 'islandora_webform_ingest') . '/includes/fix_ingest_relations.inc',
  );

  batch_set($batch);

}

/**
 * Add or replace a single child object's IW relation tags.
 *
 * @param string $pid
 *   PID for child object
 *
 * @param int $nid
 *   node id of the webform
 */
function iwi_retroactive_fix_single_iw_relation_tag($pid, $nid) {
  $object = islandora_object_load($pid);

  if ($object) {
    $object->relationships->remove(FEDORA_RELS_EXT_URI, ISLANDORA_WEBFORM_ISIW_PREDICATE);
    $object->relationships->remove(FEDORA_RELS_EXT_URI, ISLANDORA_WEBFORM_HASIW_PREDICATE);
    $object->relationships->add(ISLANDORA_RELS_EXT_URI, ISLANDORA_WEBFORM_ISIW_PREDICATE, "true", RELS_TYPE_PLAIN_LITERAL);
    $object->relationships->add(ISLANDORA_RELS_EXT_URI, ISLANDORA_WEBFORM_HASIW_PREDICATE, "{$nid}", RELS_TYPE_PLAIN_LITERAL);
  }
}

/**
 * Finished batch callback for collection predicates repair.
 */
function iwi_retroactive_fix_all_iw_relation_tags_finished($success, $results, $operations) {
  if ($success) {
    $message = format_plural(count($results), 'Relation tags repaired on one child object.', 'Relation tags repaired on @count child objects.');
  }
  else {
    $message = t('Finished with an error.');
  }
  drupal_set_message($message);
}


/**
 * Get all islandora collections as objects.
 *
 * @return mixed
 *   array of islandora objects
 */
function iwi_get_all_collection_objects() {

  module_load_include('inc', 'islandora', 'includes/utilities');

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
