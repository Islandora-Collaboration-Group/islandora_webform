<?php

/**
 * Get a list of datastream identifiers that can be ingested for this object.
 *
 * This function gets used in two contexts:
 * 1 - On the component mapping configuration form, where mimetype filtering is
 *     needed.
 * 2 - On the ingest submission page, where mimetype filtering is unnecessary.
 *
 * @param FedoraObject $object
 *   the object
 * @param array $mimetypes
 *   Used to filter datastreams by mimetype:
 *   If NULL, do not filter mimetype
 *   If string, filter mimetype by this string
 *   If array, filter mimetype by strings in the array
 *
 * @return array
 *   An array of the ingestible datastream objects
 */
function iwi_ingestible_datastreams(FedoraObject $object, $mimetypes = array('text/plain')) {

  if (!empty($object['DS-COMPOSITE-MODEL'])) {

    module_load_include('inc', 'xml_form_builder', 'includes/associations');
    module_load_include('inc', 'islandora_webform_ingest', '/includes/xml');


    $mimetypes = $mimetypes ? (array) $mimetypes : $mimetypes;
    $results = array();

    // Array for keeping track of which DSIDs we have already.
    $ingestible_dsids = array();

    // First, get available xml forms.
    // Xml forms can only accept text content.
    // If $mimetypes includes "text/plain", we can use xml forms.
    if (!$mimetypes || in_array('text/plain', $mimetypes)) {
      // Get the xml form associations and put them into the select list.
      $associations = xml_form_builder_get_associations(array(), array($object->id));
      if (count($associations) > 0) {
        foreach ($associations as $association) {
          $association['mime'] = 'text/xml';
          // Uniquely identify by dsid and form name.
          $results[$association['dsid'] . ' (' . $association['form_name'] . ')'] = $association;
          $ingestible_dsids[$association['dsid']] = $association['dsid'];
        }
      }
    }

    // Now get other datastreams and decide whether or not to add them also.
    // Run the xslt transform on the 'DS-COMPOSITE-MODEL' xml to get a list of
    // the datastreams that this content model uses.
    $info = array(
      'xsl' => drupal_get_path('module', 'islandora_webform_ingest') . '/includes/cmdatastreams.xsl',
      'input' => $object['DS-COMPOSITE-MODEL']->content,
    );
    $datastreams = (array) json_decode(islandora_webform_run_xsl_transform($info));

    foreach ($datastreams as $ds => $dsmime) {
      if (empty($ingestible_dsids[$ds])) {
        // Add ingestible datastreams from other mimetypes (e.g. text/plain)
        // If filtering by mimetype, only add datastreams that match a filter.
        if ($mimetypes) {
          foreach ($mimetypes as $type) {
            if (strpos($dsmime, $type) !== FALSE) {
              $results[$ds . ' (' . $dsmime . ')'] = array(
                'dsid' => $ds,
                'mime' => $dsmime,
              );
              $ingestible_dsids[$ds] = $ds;
            }
            // Only map the datastream once!
            break;
          }
        }
        // We aren't filtering mimetype, just add the datastream to the results.
        else {
          $results[$ds . ' (' . $dsmime . ')'] = array(
            'dsid' => $ds,
            'mime' => $dsmime,
          );
          $ingestible_dsids[$ds] = $ds;
        }
      }
    }

    return $results;
  }
}

/**
 * Find eligible datastream mimetypes for a given component type.
 *
 * @param array $component
 *   the webform component
 *
 * @return array
 *   an array of mimetypes that this component could be mapped to
 */
function iwi_datastream_mime_from_component($component) {

  // File components.
  $types = iwi_file_extensions_from_component($component);
  if (count($types)) {
    return iwi_get_mimetypes_from_extensions($types);
  }

  // For other component types, just use this hard-coded mapping.
  $map = array(
    'datepicker' => array('text/plain'),
    'date' => array('text/plain'),
    'email' => array('text/plain'),
    'hidden' => array('text/plain'),
    'number' => array('text/plain'),
    'select' => array('text/plain'),
    'textarea' => array('text/plain'),
    'html_textarea' => array('text/html'),
    'textfield' => array('text/plain'),
    'time' => array('text/plain'),
    'tag' => array('text/plain'),
  );
  return isset($map[$component['type']]) ? $map[$component['type']] : array('application/octet-stream');
}

/**
 * Utility function to get file extension filters defined for file component.
 *
 * @param array $component
 *   a webform component
 *
 * @return array
 *   an array of file extensions which may be empty
 */
function iwi_file_extensions_from_component($component) {
  $types = array();
  if (!empty($component['extra']['filtering']['addextensions'])) {
    $types = array_map('trim', explode(',', $component['extra']['filtering']['addextensions']));
  }
  if (!empty($component['extra']['filtering']['types'])) {
    $types = array_merge($types, $component['extra']['filtering']['types']);
  }

  return $types;
}

/**
 * Convert form array and into a list of ingestible fields and field types.
 *
 * Fieldsets are returned as arrays of fields recursively.
 *
 * @param array $input_form
 *   a drupal form
 *
 * @return array|bool
 *   List of ingestible fields. Fieldsets are returned as arrays of
 *   fields recursively
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
        'hidden',
      ))
    ) {
      continue;
    }

    // If the child has children, recursively search in it.
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
    return FALSE;
  }
}

/**
 * Get an array of field paths for a given form.
 *
 * @param array $form
 *   form
 *
 * @return array
 *   a list of field paths
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
 * Recursive function builds field paths for nested fields from form data.
 *
 * @param array $pathstack
 *   An array passed by reference used to hold the working path stack
 * @param array $paths
 *   An array passed by reference that is populated with the paths
 * @param array $data
 *   The form data
 * @param string $delimiter
 *   What we're inserting in between field names in the path
 */
function iwi_path_stack(&$pathstack, &$paths, $data, $delimiter = ':') {
  foreach ($data as $rowname => $rowdata) {
    array_push($pathstack, $rowname);
    if (is_array($rowdata)) {
      iwi_path_stack($pathstack, $paths, $rowdata, $delimiter);
    }
    else {
      $mimes = iwi_datastream_mime_from_component(array('type' => $rowdata));
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
 * Get the islandora_webform_ingestion records for a submission.
 *
 * @param int $sid
 *   The webform submission id
 *
 * @return mixed
 *   An array of one or more standard objects representing individual
 *   database rows or NULL for no result.
 */
function iwi_get_ingestion_info($sid) {
  return db_select('islandora_webform_ingestions', 'i')
    ->fields('i')
    ->condition('i.sid', $sid)
    ->execute()
    ->fetchAll();
}

/**
 * A webform component is being saved, save its islandora ingest mapping.
 *
 * @param array $form
 *   the component edit form
 * @param array $form_state
 *   the form_state
 *
 * @throws \Exception
 */
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
    // Empty mode means "do not ingest", so remove the mapping.
    if (empty($mode)) {
      db_delete('islandora_webform_ingest_map')
        ->condition('id', $id)
        ->execute();
    }
    else {
      db_update('islandora_webform_ingest_map')
        ->fields(array('data' => serialize($data)))
        ->condition('id', $id)
        ->execute();
    }
  }
  elseif (!empty($mode)) {
    db_insert('islandora_webform_ingest_map')
      ->fields(array(
          'nid' => $nid,
          'cid' => $cid,
          'data' => serialize($data),
        )
      )->execute();
  }

}

/**
 * Retrieve islandora ingest mapping for a given webform component.
 *
 * @param int $webform_nid
 *   The node id of the webform
 * @param int $cid
 *   The component id
 *
 * @return mixed
 *   the component mapping data as an array
 */
function iwi_get_saved_component_mapping($webform_nid, $cid) {
  $data = db_select('islandora_webform_ingest_map', 'm')
    ->fields('m', array('data'))
    ->condition('m.nid', $webform_nid)
    ->condition('m.cid', $cid)
    ->execute()->fetchField();
  return @unserialize($data);
}

/**
 * Retrieve all islandora ingest mappings for a given webform.
 *
 * @param int $webform_nid
 *   the node id of the webform
 *
 * @return mixed
 *   an array with one row per component,
 *   each row as an array of component mapping data
 */
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

/**
 * Retrieve the content_model filter for a webform.
 *
 * @param int $webform_nid
 *   the node id of the webform
 *
 * @return string
 *   the cmodel filter value, or NULL
 */
function iwi_webform_content_model($webform_nid) {
  return db_select('islandora_webform_webforms', 'iw')
    ->fields('iw', array('cmodel_filter'))
    ->condition('iw.entity_id', $webform_nid)
    ->execute()->fetchField();
}

/**
 * Retrieve the ingest settings for a webform.
 *
 * @param int $webform_nid
 *   the node id of the webform
 *
 * @return string
 *   the cmodel filter value, or NULL
 */
function iwi_webform_ingest_settings($webform_nid) {
  return db_select('islandora_webform_ingest_webforms', 'iw')
    ->fields('iw', array('ingest_enabled', 'cmodel', 'relation', 'namespace'))
    ->condition('iw.nid', $webform_nid)
    ->execute()->fetch();
}


/**
 * This function is the clone & replace workhorse.
 *
 * It uses the objective_form_storage to allow new elements to behave like
 * those created directly using XML values and elements.
 *
 * @param array $form
 *   the form we are modifying
 * @param array $form_state
 *   the form state
 * @param array $cloned_elements
 *   previous cloned elements so we don't clone and reclone.
 * @param string $elementspath
 *   the full path(in case of multivalued, theorical) to the target element.
 * @param array $input_value
 *   the passed arraay of attributes for the target element
 * @param string $mode
 *   either "append" or "replace" (default)
 * @param string $delimiter
 *   the character used to delimit the $elementspath
 */
function iwi_set_form_element_byhash(&$form, &$form_state, &$cloned_elements, $elementspath, $input_value, $mode = 'replace', $delimiter = ':') {

  $parents = explode($delimiter, $elementspath);
  // XML forms have two special form elements, whose children need to be cloned:
  // if #type in (tags,tabs) and $mode=='append'
  // So we are going to check for those types before deciding how
  // to handle appends.

  $special_xmlform_elements = array('tags' => 'tag', 'tabs' => 'tabpanel');

  // We are handling append different, because replace means just setting the
  // value to an existing element. But if the element was already cloned we
  // will have to switch to a replace functionality.
  // This means: if we got i.e a submited field like name[0]manePart and then
  // later a name[0]role we can't clone again. So we store the first clone
  // element, and just add the value to this one.
  switch ($mode) {
    case 'append':
      $ref = &$form;
      $temp_path = array();
      $special_element_child_type = NULL;
      $target_element_hash = NULL;
      $child_element_hash = NULL;
      $level = 0;
      $special_element_ref = array();
      $previous_partial_path = array();
      $key_exists = NULL;
      foreach ($parents as $parent) {
        if (is_array($ref) && array_key_exists($parent, $ref)) {
          $key_exists = TRUE;
          $level++;
          $temp_path[] = $parent;
          if (count($special_element_ref) > 0 && $ref[$parent]['#type'] == $special_element_child_type) {
            // Means we already have found an special element container in a
            // previous iteration and we are standing at a child element;

            // Keep track of where we are, store tree descending path.
            $future_partial_path = array_slice($parents, $level);
            // Tree up path.
            $previous_partial_path = $temp_path;
            // Getting rid of the "numeric key" if it's there...
            array_pop($previous_partial_path);
            // Lets get the hash!
            $child_element_hash = $ref[$parent]['#hash'];
          }
          if (array_key_exists($ref[$parent]['#type'], $special_xmlform_elements)) {
            // Means we are at the root element of a special xml form, we want
            // the last one always, in the rare case we have multiple nested
            // ones.
            $special_element_ref = &$ref[$parent];
            $special_element_child_type = $special_xmlform_elements[$ref[$parent]['#type']];
            // Lets store this point so we know where to append our "cloned"
            // element.
            $target_element_hash = $ref[$parent]['#hash'];
          }

          $ref = &$ref[$parent];
        }
        else {
          // We passed an invalid path.
          $key_exists = FALSE;
          drupal_set_message(t('Attempted to set a value at %path in this form, but it does not exist.', array('%path' => $elementspath)), 'error');
        }
      }
      if (($key_exists && !empty($target_element_hash)) && (!empty($child_element_hash))) {
        // OK, we got something to clone, let's do it using the objective_
        // form_api:
        $element = $form_state['storage']['objective_form_storage']['root']->findElement($target_element_hash);
        if ($element) {
          switch ($special_element_child_type) {
            case "tabpanel":
              $tab = $element->findElement($child_element_hash);
              if ($tab) {
                if (!array_key_exists($child_element_hash, $cloned_elements)) {
                  // If not already cloned clone it.
                  $new_tab = clone $tab;

                  $set_element_properties = function ($element) {
                    $element->default_value = NULL;
                  };
                  // Set element properties on the descendants.
                  $new_tab->eachDecendant($set_element_properties);

                  $element->adopt($new_tab);
                  $new_tab_drupal = $new_tab->toArray();

                  drupal_array_set_nested_value($new_tab_drupal, array_merge($future_partial_path, array('#attributes')), $input_value['#attributes']);
                  drupal_array_set_nested_value($new_tab_drupal, array_merge($future_partial_path, array('#default_value')), $input_value['#default_value']);

                  $special_element_ref[] = $new_tab_drupal;

                  // Appending the drupal form array back to where it belongs.
                  // Lets store where do we put this one, so we can later just
                  // fill out the values if we need to.
                  $special_element_children = element_children($special_element_ref);
                  $cloned_elements[$child_element_hash] = end($special_element_children);
                }
                else {
                  // Means we have already clone the container, so just put
                  // the values there. We can't use the hash here; we don't
                  // know it.

                  // This gives us the real path to the previously cloned
                  // element.
                  $fullnewpath = array_merge($previous_partial_path, array($cloned_elements[$child_element_hash]), $future_partial_path);
                  // We could also just get the last element_children of the
                  // container, and add there...
                  drupal_array_set_nested_value($form, array_merge($fullnewpath, array('#attributes')), $input_value['#attributes']);
                  drupal_array_set_nested_value($form, array_merge($fullnewpath, array('#default_value')), $input_value['#default_value']);
                }
              }
              break;

            case "tag":
              // Now appending at the end. This means the new value is
              // "themed" like a tag. Should, in case of having no previous
              // value, just add the value to the existing input ?([0])
              // ..it's like replacing...
              $form_children = element_children($special_element_ref);
              $input_field = &$special_element_ref[array_shift($form_children)];
              $input_field['#value'] = '';
              $element_children = array_values($element->children);
              $input = array_shift($element_children);
              $new_tag = clone $input;
              // Create new tag and have it stored in the state.
              $input->parent->adopt($new_tag);
              $new_tag_drupal = $new_tag->toArray();
              $new_tag_drupal['#default_value'] = $input_value['#default_value'];
              // Update drupal form.
              $special_element_ref[] = $new_tag_drupal;
              break;

            default:
              break;
          }
        }
      }
      else {
        // We are going to text-append existing value.
        $key_exists = NULL;
        $target_element_hash = drupal_array_get_nested_value($form, array_merge($parents, array('#hash')), $key_exists);
        if ($key_exists) {
          drupal_array_set_nested_value($form, array_merge($parents, array('#attributes')), $input_value['#attributes']);
          drupal_array_set_nested_value($form, array_merge($parents, array('#default_value')), $input_value['#default_value']);
          $element = $form_state['storage']['objective_form_storage']['root']->findElement($target_element_hash);
          if (!empty($element->default_value)) {
            $input_value['#default_value'] = "; " . $input_value['#default_value'];
          }
          $element->default_value = $input_value['#default_value'];
        }

      }
      break;

    default:
      // We are going to replace existing value.
      // If we got only one, just put our values there.
      $key_exists = NULL;
      $target_element_hash = drupal_array_get_nested_value($form, array_merge($parents, array('#hash')), $key_exists);
      if ($key_exists) {
        drupal_array_set_nested_value($form, array_merge($parents, array('#attributes')), $input_value['#attributes']);
        drupal_array_set_nested_value($form, array_merge($parents, array('#default_value')), $input_value['#default_value']);
        $element = $form_state['storage']['objective_form_storage']['root']->findElement($target_element_hash);
        $element->default_value = $input_value['#default_value'];
      }
  }
}

/**
 * Write webform ingest configuration to database.
 *
 * @param array $form
 *   The webform config form
 * @param array $form_state
 *   The webform config form_state
 *
 * @throws \Exception
 */
function iwi_save_webform_configuration($form, &$form_state) {

  $nid = $form['nid']['#value'];
  $enabled = !empty($form_state['values']['islandora_ingest']['ingest_enabled']) ? $form_state['values']['islandora_ingest']['ingest_enabled'] : 0;
  $cmodel = !empty($form_state['values']['islandora_ingest']['cmodel']) ? $form_state['values']['islandora_ingest']['cmodel'] : '';
  $relation = !empty($form_state['values']['islandora_ingest']['relation']) ? $form_state['values']['islandora_ingest']['relation'] : 'child';
  $namespace = !empty($form_state['values']['islandora_ingest']['namespace']) ? $form_state['values']['islandora_ingest']['namespace'] : 'islandora';
  if ($old_settings = db_select('islandora_webform_ingest_webforms', 'm')
    ->fields('m')
    ->condition('m.nid', $nid)
    ->execute()->fetch()
  ) {

    // If content model is changing, wipe out ingest mappings.
    if ($old_settings->cmodel != $cmodel) {
      db_delete('islandora_webform_ingest_map')->condition('nid', $nid)->execute();
      drupal_set_message(t('The destination content model was changed and therefore all existing component ingest mappings for this webform were deleted. You will have to edit each webform component that you wish to ingest.'), 'warning');
    }

    // If the relation is changing, update the relations on any
    // objects previously assigned the original relation term.
    if ($old_settings->relation != $relation) {

      // Find all objects with the old relation if it is not blank.
      if ($old_settings->relation) {
        module_load_include('inc', 'islandora_webform', 'includes/utilites');
        $child_objects_data = iwi_query_objects_with_relation($old_settings->relation);

        // Loop through the child (related) objects, updating their rels-ext
        // to use the new relation term.
        foreach ($child_objects_data as $child_object_data) {
          $child_object = islandora_object_load($child_object_data['object']['value']);

          // Get the old relation record.
          $old_relations = $child_object->relationships->get('info:fedora/fedora-system:def/relations-external#', $old_settings->relation);
          // In theory, there can be multiple relations matching this term,
          // so we have to loop.
          foreach ($old_relations as $old_relation) {

            // Get the parent object pid from the old relation.
            $old_relation_pid = $old_relation['object']['value'];

            // If new relation is not blank, add it, using the old relation's
            // parent object pid.
            if ($relation) {
              $child_object->relationships->add('info:fedora/fedora-system:def/relations-external#', $relation, $old_relation_pid);
            }

            // In any case, remove the old relation.
            $child_object->relationships->remove('info:fedora/fedora-system:def/relations-external#', $old_settings->relation, $old_relation_pid);
          }

          drupal_set_message(format_string('@count objects had their relations updated from "@old" to "@new"', array(
            '@count' => count($child_objects_data),
            '@old' => $old_settings->relation,
            '@new' => $relation,
          )));
        }
        if ($relation) {
          // Update the block config to use the new relation as the delta.
          db_update('block')
            ->fields(array('delta' => $relation))
            ->condition('delta', $old_settings->relation)
            ->condition('module', 'islandora_webform_ingest')
            ->execute();
        }
        else {
          // New relation is blank, so delete the related objects block
          // configuration.
          db_delete('block')
            ->condition('delta', $old_settings->relation)
            ->condition('module', 'islandora_webform_ingest')
            ->execute();
        }
      }
    }

    // Update the previous islandora webform ingest settings record.
    db_update('islandora_webform_ingest_webforms')
      ->fields(array(
        'ingest_enabled' => $enabled,
        'cmodel' => $cmodel,
        'relation' => $relation,
        'namespace' => $namespace,
      ))
      ->condition('id', $old_settings->id)
      ->execute();
  }

  // No previous islandora webform ingest settings record, so just create a new
  // one.
  else {
    db_insert('islandora_webform_ingest_webforms')
      ->fields(array(
        'nid' => $nid,
        'ingest_enabled' => $enabled,
        'cmodel' => $cmodel,
        'relation' => $relation,
        'namespace' => $namespace,
      ))
      ->execute();
  }
}

/**
 * Utility function returns mime types given file extensions.
 *
 * @param string|array $extensions
 *   a single file extension as a string, or multiple file extensions
 *   as an array.
 *
 * @return string|array
 *   mime type(s), defaults to 'application/octet-stream'
 */
function iwi_get_mimetypes_from_extensions($extensions) {
  module_load_include('inc', 'content', 'includes/file.mimetypes');
  $mapping = file_mimetype_mapping();
  $mode = is_array($extensions) ? 'array' : 'string';
  $extensions = (array) $extensions;
  $mimetypes = array();
  foreach ($extensions as $extension) {
    // Islandora doesn't use the standard jpeg mime type.
    if ($extension == 'jpg') {
      $mimetypes[] = 'image/jpg';
    }
    elseif (isset($mapping['extensions'][$extension])) {
      $mimetypes[] = $mapping['mimetypes'][$mapping['extensions'][$extension]];
    }
  }
  if (!count($mimetypes)) {
    $mimetypes[] = 'application/octet-stream';
  }
  return $mode == 'array' ? $mimetypes : reset($mimetypes);
}

/**
 * Get data about objects associated with this pid through any rels-ext.
 *
 * @TODO: Add exclusive option - only return objects whose only relation
 * is to this object.
 *
 * @param int $pid
 *   The parent object's pid.
 *
 * @param int $page_number
 *   Used to calculate offset
 *
 * @param int $page_size
 *   If > 0, limit count of results to this number
 *
 * @return array
 *   An array containing pids of child objects
 */
function iwi_query_related_objects($pid, $page_number = 0, $page_size = 0) {
  $connection = islandora_get_tuque_connection();
  $query = <<<EOQ
SELECT ?object ?predicate
FROM <#ri>
WHERE {
  ?object <fedora-model:label> ?title ;
  ?predicate <info:fedora/{$pid}> .
  !optionals
  !optexclude
  !filters
}

EOQ;
  // Be consistent with REPO's XACML permissions.
  $query_optionals = (array) module_invoke('islandora_xacml_api', 'islandora_basic_collection_get_query_optionals', 'manage');
  $query_filters = (array) module_invoke('islandora_xacml_api', 'islandora_basic_collection_get_query_filters');
  // Exclude collection predicates here.
  $collection_predicate_filters = " !sameTerm(?predicate, <fedora-rels-ext:isMemberOfCollection>) && !sameTerm(?predicate, <fedora-rels-ext:isMemberOf>)";
  $query_filters[] = $collection_predicate_filters;
  $filter_map = function ($filter) {
    return "FILTER($filter)";
  };
  // Basicly this query statement exludes every object that also has a
  // relationship to a collection. This avoids deleting object that would not
  // really became orphan if deleting it's parent.
  // @TODO we should really extend this to any relation that connects this
  // object to any other different than $PID.
  $query_unbound_optionals = "?object2 <fedora-model:label> ?title ; ?predicate <info:fedora/{$pid}>. ?object2 <fedora-rels-ext:isMemberOfCollection> ?collection";
  $query_filters[] = "!bound(?collection)";

  $query = format_string($query, array(
    '!optionals' => !empty($query_optionals) ? ('OPTIONAL {{' . implode('} UNION {', $query_optionals) . '}}') : '',
    '!filters' => implode(' ', array_map($filter_map, $query_filters)),
  ));
  $query = format_string($query, array(
    '!optexclude' => !empty($query_unbound_optionals) ? ('. OPTIONAL { ' . $query_unbound_optionals . '}') : '',
  ));

  // Include paging parameters if provided.
  if ($page_number > 0 && $page_size > 0) {
    // Add in the offset.
    $offset = $page_number * $page_size;
    $query .= " offset $offset";
  }
  if ($page_size > 0) {
    // Add in the limit.
    $query .= " limit $page_size";
  }
  error_log($query);
  $queryresults = $connection->repository->ri->sparqlQuery($query);

  return $queryresults;
}

/**
 * Get data about objects having this relation term.
 *
 * @param string $relation
 *   The relation term.
 *
 * @return array
 *   An array containing query results
 */
function iwi_query_objects_with_relation($relation) {
  $connection = islandora_get_tuque_connection();
  $query = <<<EOQ
PREFIX islandora-rels-ext: <http://islandora.ca/ontology/relsext#>
PREFIX fedora-rels-ext: <info:fedora/fedora-system:def/relations-external#>
SELECT ?object ?predicate ?secondobject
FROM <#ri>
WHERE {
  ?object <fedora-model:label> ?title ;
  ?predicate ?secondobject .
FILTER(sameTerm(?predicate, <fedora-rels-ext:{$relation}>))}
EOQ;

  $queryresults = $connection->repository->ri->sparqlQuery($query);

  return $queryresults;
}
