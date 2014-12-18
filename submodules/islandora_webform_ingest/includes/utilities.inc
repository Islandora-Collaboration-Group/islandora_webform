<?php

/**
 * Get a list of datastream identifiers that can be ingested for this object
 *
 * @param IslandoraFedoraObject $object
 * @param MIXED $mimetype
 *  Used to filter datastreams by mimetype
 *  If NULL, do not filter mimetype
 *  If string, filter mimetype by this string
 *  If array, filter mimetype by strings in the array
 * @return array $results
 *  An array of the ingestable datastream objects
 */
function iwi_ingestable_datastreams(IslandoraFedoraObject $object, $mimetypes = array('text/plain')) {

  if (!empty($object['DS-COMPOSITE-MODEL'])) {

    module_load_include('inc', 'xml_form_builder', 'includes/associations');
    module_load_include('inc', 'islandora_webform_ingest', '/includes/xml');

    $info = array(
      'xsl' => drupal_get_path('module', 'islandora_webform_ingest') . '/includes/cmdatastreams.xsl',
      'input' => $object['DS-COMPOSITE-MODEL']->content,
    );

    // Run the xslt transform on the 'DS-COMPOSITE-MODEL' xml to get a list of the datastreams
    // that this content model uses.
    $datastreams = (array) json_decode(islandora_webform_run_xsl_transform($info));

    $mimetypes = (array) $mimetypes;
    $results = array();

    // Build the list of ingestable datastreams. If datastream is text/xml, ingestability
    // depends on existance of form associations. There can be more than one.
    foreach ($datastreams as $ds => $dsmime) {

      // Datastream IDs can have zero, one or many form associations. Add these to
      // the list first.
      $associations = xml_form_builder_get_associations(array(), array($object->id), array($ds));
      if (count($associations) > 0) {
        foreach ($associations as $association) {
          $association['mime'] = $dsmime;
          // Uniquely identify by dsid and form name
          $results[$ds . ' (' . $association['form_name'] . ')'] = $association;
        }
      }

      // Add ingestable datastreams from other mimetypes (e.g. text/plain)
      foreach ($mimetypes as $type) {
        if (strpos($dsmime, $type) !== FALSE) {
          $results[$ds . ' (' . $dsmime . ')'] = array(
            'dsid' => $ds,
            'mime' => $dsmime,
          );
          break;
        }
      }
    }

    return $results;
  }
}

/**
 * Find eligible datastream mimetypes for a given component type
 *
 * @param $component_type
 * @return array
 */
function iwi_datastream_mime_from_component_type($component_type) {
  $map = array(
    'date' => array('text/plain'),
    'email' => array('text/plain'),
    'file' => array('image/jpeg', 'application/pdf'),
    'hidden' => array('text/plain'),
    'number' => array('text/plain'),
    'select' => array('text/plain'),
    'textarea' => array('text/plain'),
    'textfield' => array('text/plain'),
    'time' => array('text/plain'),
    'tag' => array('text/plain'),
  );
  return isset($map[$component_type]) ? $map[$component_type] : array();
}


/**
 * Utility function takes a form array and returns a list of fields and field
 * types that we support ingesting to. Fieldsets are returned as arrays of fields
 * recursively.
 *
 * @param array $input_form
 * @return array|bool
 */
function iwi_recursive_form_parsing(array $input_form) {

  if (!is_array($input_form)) {
    return FALSE;
  }
  $visible_children = array();

  foreach (element_children($input_form) as $key) {
    $child = $input_form[$key];

    // Skip un-accessible children.
    if (isset($child['#access']) && !$child['#access']) {
      continue;
    }

    // Skip value and hidden elements, since they are not rendered.
    if (isset($child['#type']) && in_array($child['#type'], array(
        'value',
        'hidden'
      ))
    ) {
      continue;
    }

    // If the child has children, recursively search in it
    $child_children = element_children($child);

    if ((is_array($child)) && (!empty($child)) && count($child_children) > 0) {
      $temp = iwi_recursive_form_parsing($child);
      if ($temp) {
        $visible_children[$key] = $temp;
      }
      continue;
    }
    if (!empty($child['#actions']['update'])) {
      $visible_children[$key] = $child['#type'];
    }

  }
  if (!empty($visible_children)) {
    return $visible_children;
  }
  else {
    return false;
  }
}

/**
 * Get an array of field paths for a given form
 *
 * @param $form
 * @return array
 */
function iwi_form_field_paths($form) {
  $data = iwi_recursive_form_parsing($form);
  $pathstack = array();
  $paths = array();
  $delimiter = ':';
  iwi_path_stack($pathstack, $paths, $data, $delimiter);

  return $paths;
}

/**
 * Recursive function builds field paths for nested fields from form data
 *
 * @param $pathstack
 *  An array passed by reference used to hold the working path stack
 *  --
 * @param $paths
 *  - An array passed by reference that is populated with the paths
 * @param $data
 *  - The form data
 * @param string $delimiter
 *  - What we're inserting in between field names in the path
 */
function iwi_path_stack(&$pathstack, &$paths, $data, $delimiter = ':') {
  foreach ($data as $rowname => $rowdata) {
    array_push($pathstack, $rowname);
    if (is_array($rowdata)) {
      iwi_path_stack($pathstack, $paths, $rowdata, $delimiter);
    }
    else {
      $mimes = iwi_datastream_mime_from_component_type($rowdata);
      if (count($mimes)) {
        $paths[] = array(
          'field path' => implode($delimiter, $pathstack),
          'mimes' => $mimes,
        );
      }
    }
    array_pop($pathstack);
  }
}

/**
 * Utility function which gets the islandora_webform_ingestion records for a
 * given webform submission.
 *
 * @param $sid
 *  The webform submission id
 *
 * @return mixed
 *  An array of one or more standard objects representing individual database rows
 *  or NULL for no result.
 */
function iwi_get_ingestion_info($sid) {
  return db_select('islandora_webform_ingestions', 'i')
    ->fields('i')
    ->condition('i.sid', $sid)
    ->execute()
    ->fetchAll();
}

function iwi_save_component_mapping($form, $form_state) {

  $nid = $form['nid']['#value'];
  $cid = $form['cid']['#value'];
  $mode = !empty($form_state['values']['ingest']['mode']) ? $form_state['values']['ingest']['mode'] : '';
  $datastream_label = !empty($form_state['values']['ingest']['datastream']) ? $form_state['values']['ingest']['datastream'] : '';
  $datastream_parts = explode(' ', $datastream_label);
  $datastream_id = $datastream_parts[0];
  $field = !empty($form_state['values']['ingest']['fields_wrapper']['fields']) ? $form_state['values']['ingest']['fields_wrapper']['fields'] : '';
  $data = array(
    'mode' => $mode,
    'datastream' => $datastream_label,
    'datastream_id' => $datastream_id,
    'field' => $field,
  );
  if ($id = db_select('islandora_webform_ingest_map', 'm')
    ->fields('m', array('id'))
    ->condition('m.nid', $nid)
    ->condition('m.cid', $cid)
    ->execute()->fetchField()
  ) {
    db_update('islandora_webform_ingest_map')
      ->fields(array('data' => serialize($data)))
      ->condition('id', $id)
      ->execute();
  }
  else {
    db_insert('islandora_webform_ingest_map')
      ->fields(array(
          'nid' => $nid,
          'cid' => $cid,
          'data' => serialize($data),
        )
      )->execute();
  }

}

function iwi_get_saved_component_mapping($webform_nid, $cid) {
  $data = db_select('islandora_webform_ingest_map', 'm')
    ->fields('m', array('data'))
    ->condition('m.nid', $webform_nid)
    ->condition('m.cid', $cid)
    ->execute()->fetchField();
  return @unserialize($data);
}

function iwi_get_saved_component_mappings($webform_nid) {
  $results = db_select('islandora_webform_ingest_map', 'm')
    ->fields('m', array('cid', 'data'))
    ->condition('m.nid', $webform_nid)
    ->execute()->fetchAll();
  foreach ($results as &$result) {
    $result->data = @unserialize($result->data);
  }
  return $results;
}

function iwi_webform_content_model($webform_nid) {
  return db_select('islandora_webform_webforms', 'iw')
    ->fields('iw', array('cmodel_filter'))
    ->condition('iw.entity_id', $webform_nid)
    ->execute()->fetchField();
}

/**
 * Recursive function that inserts a value into a multidimensional array at
 * a location defined by a path
 *
 * @param array $array_ptr
 * @param string $path
 * @param mixed $value
 * @param string $delimiter
 * @param boolean $merge
 */
function iwi_array_path_set_value(&$array_ptr, $path, $value, $delimiter = ':', $merge = TRUE) {

  $keys = explode($delimiter, $path);
  // extract the last key
  $last_key = array_pop($keys);

  // walk/build the array to the specified key
  foreach($keys as $arr_key) {
    if (!array_key_exists($arr_key, $array_ptr)) {
      $array_ptr[$arr_key] = array();
    }
    $array_ptr = &$array_ptr[$arr_key];
  }

  // set the final key
  if($merge && isset($array_ptr[$last_key]) && is_array($array_ptr[$last_key])) {
    $array_ptr[$last_key] = array_merge($array_ptr[$last_key], (array) $value);
  }
  else {
    $array_ptr[$last_key] = $value;
  }
}