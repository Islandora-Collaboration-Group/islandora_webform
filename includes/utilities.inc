<?php

/**
 * @file
 * Miscellaneous functions
 */


/**
 * Return a list those models that are content models
 *
 * Expects a content models array produced by $object->models
 * (array values are the pids for those content models)
 *
 * @param $models
 * @return array
 */
function _iw_content_models($models) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  return array_intersect(array_keys(islandora_get_content_models()), array_values($models));
}


/**
 * Get a list of object pids and their labels found in this repository.
 *
 * Filter by type: 'collections' or 'cmodels'
 *
 * @string $type
 *  'collections' or 'cmodels'
 * @return array
 *  A list of object pids and their labels
 */
function iw_get_collections_or_content_models($type) {
  $types = array(
    'collections' => 'islandora:collectionCModel',
    'cmodels' => 'fedora-system:ContentModel-3.0'
  );
  $type_for_query = '<info:fedora/' . $types[$type] . '>';
  // Query the rindex to get all collections.
  $tuque = islandora_get_tuque_connection();
  $ri_query = "select \$object \$label from <#ri>
     where \$object <fedora-model:hasModel> $type_for_query
      and \$object <fedora-model:label> \$label";
  $results = $tuque->repository->ri->itqlQuery($ri_query, 'unlimited');
  $ret = array();
  foreach ($results as $member) {
    $pid = preg_replace('/info:fedora\//', '', $member['object']['value']);
    $label = $member['label']['value'];
    // We don't want the root collection.
    if ($pid == 'islandora:root') {
      $label = 'Islandora Root Collection';
    }
    $ret[$pid] = $label;
  }
  return $ret;
}


/**
 * Get a list of enabled solution packs
 *
 * @return array
 */
function _iw_get_solution_packs() {
  $solution_packs = &drupal_static(__FUNCTION__);
  if (!isset($solution_packs)) {
    $solution_packs = cache_get('iw_solution_packs');
    if (empty($solution_packs)) {
      $connection = islandora_get_tuque_connection();

      // All solution packs implement hook_islandora_required_objects
      $result = module_invoke_all('islandora_required_objects', $connection);
      $solution_packs = array_keys($result);
      cache_set('iw_solution_packs', $solution_packs, CACHE_TEMPORARY);
    }
  }
  return $solution_packs;
}

/**
 * Get a list of all islandora_webforms applicable to a given IslandoraFedoraObject
 *
 * @param IslandoraFedoraObject $object
 * @return mixed
 */
function islandora_webform_object_get_webforms(IslandoraFedoraObject $object) {

  if (get_class($object) == 'IslandoraFedoraObject') {

    $pid = $object->id;
    $in_cmodels = _iw_content_models($object->models);
    $in_collections = $object->getParents();
    $sql = "SELECT w.*, n.title as title FROM {islandora_webform_webforms} w JOIN {node} n ON (n.nid = w.entity_id)
            WHERE w.enabled = :enabled AND ( :pid REGEXP w.pid_filter OR w.pid_filter = '' )
            AND ( w.cmodel_filter = '' OR w.cmodel_filter IN (:cmodels) )
            ";
    $query_args = array(
      ':enabled' => 1,
      ':pid' => $pid,
      ':cmodels' => $in_cmodels,
    );

    // If the object is in a collection, then add the collections filter
    if (!empty($in_collections)) {
      $sql .= "AND ( w.collection_filter = '' OR w.collection_filter IN (:collections) )";
      $query_args[':collections'] = $in_collections;
    }
    // If the object is not a member of a collection,
    // then filter by collection_filter is "Any"
    else {
      $sql .= "AND ( w.collection_filter = '' )";
    }

    $result = db_query($sql, $query_args);

    return $result->fetchAll();
  }
}


/**
 * Get a list of all islandora_webforms applicable to a given PID
 *
 * @param string $pid
 * @return mixed
 */
function islandora_webform_pid_get_webforms($pid) {
  $object = islandora_object_load($pid);
  if ($object) {
    return islandora_webform_object_get_webforms($object);
  }
  else {
    return FALSE;
  }
}

/**
 * Get the islandora configuration for a given webform
 *
 * @param $node
 * @return mixed
 */
function iw_node_islandora_webform($node) {

  return db_select('islandora_webform_webforms', 'w')->fields('w')->condition('w.entity_id', $node->nid)->execute()->fetch();

}
