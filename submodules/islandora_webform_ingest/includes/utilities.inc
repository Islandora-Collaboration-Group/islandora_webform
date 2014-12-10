<?php

/**
 * Get a list of datastream ids that can be ingested to
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
function iwi_ingestable_datastreams(IslandoraFedoraObject $object, $mimetypes = array()) {

  if (!empty($object['DS-COMPOSITE-MODEL'])) {

    $info = array(
      'xsl' => drupal_get_path('module', 'islandora_webform_ingest') . '/includes/cmdatastreams.xsl',
      'input' => $object['DS-COMPOSITE-MODEL']->content,
    );

    module_load_include('inc', 'xml_form_builder', 'includes/associations');


    module_load_include('inc', 'islandora_webform_ingest', '/includes/xml');
    $content_models = (array) json_decode(islandora_webform_run_xsl_transform($info));
    $mimetypes = (array) $mimetypes;
    $results = array();
    foreach ($content_models as $ds => $dsmime) {
      $associations = xml_form_builder_get_associations(array(), array($object->id), array($ds));
      if (count($associations) > 0) {
        foreach ($associations as $association) {
          $association['mime'] = $dsmime;
          $results[$ds . ' (' . $association['form_name'] . ')'] = $association;
        }
      }
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
  );
  return $map[$component_type];
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
  $visible_children = array();

  foreach (element_children($input_form) as $key) {
    $child = $input_form[$key];

    // Skip un-accessible children.
    if (isset($child['#access']) && !$child['#access']) {
      continue;
    }

    // Skip value and hidden elements, since they are not rendered.
    if (isset($child['#type']) && in_array($child['#type'], array('value', 'hidden'))) {
      continue;
    }

    // If the child has children, recursively search in it
    $child_children = element_children($child);
    if ((is_array($child)) && (!empty($child)) && $child['#type'] == 'fieldset' && count($child_children) > 0) {
      $temp = iwi_recursive_form_parsing($child);
      if($temp) {
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

function iwi_form_field_paths($form) {
  $data = iwi_recursive_form_parsing($form);
  $pathstack = array();
  $paths = array();
  $delimiter = ':';
  iwi_path_stack($pathstack, $paths, $data, $delimiter);

  return $paths;
}

function iwi_path_stack(&$pathstack, &$paths, $data, $delimiter = ':') {
  foreach($data as $rowname => $rowdata) {
    array_push($pathstack, $rowname);
    if (is_array($rowdata))  {
      iwi_path_stack($pathstack, $paths, $rowdata, $delimiter);
		}
    else {
      $paths[] = array(
        'field path' => implode($delimiter, $pathstack),
        'mimes' => iwi_datastream_mime_from_component_type($rowdata),
      );
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
  return db_select('islandora_webform_ingestions','i')->fields('i')->condition('i.sid', $sid)->execute()->fetchAll();
}