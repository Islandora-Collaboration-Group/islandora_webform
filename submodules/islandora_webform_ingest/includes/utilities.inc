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
 * iwi_array_path_set_value($aggregate_form[$dsid][$dsid], $mapping->data['field'], $field_submitted_value, $mapping->data['mode']);
 */
function iwi_array_path_set_value(&$array_ptr, $path, $value, $mode = 'replace', $delimiter = ':', $merge = TRUE) {
  //Not used any more by me. Still left it, because it's so pretty done and could be used later!
  $keys = explode($delimiter, $path);

  // If there is a zero in the path, this is a multiple-value field.
  $multi_index = array_search('0', $keys);

  // figure out the last key
  end($keys);

  // The last key is a multi-value field if zero is the last key
  $last_key_multi = $multi_index === key($keys);

  // Pop off the last key; it gets handled differently at the end
  $last_key = array_pop($keys);

  // walk/build the form array up to but not including the last key
  foreach ($keys as $arr_index => $arr_key) {
    if (!array_key_exists($arr_key, $array_ptr)) {
      $array_ptr[$arr_key] = array();
    }

    // Look to see if the section of the form we are on needs to be cloned
    if ($arr_index === $multi_index && $mode == 'append') {
      // Prepending a multi-value sub-form, clone the sub-form and add it to the end.
      // We will push our values into the original.
      array_push($array_ptr, arrayCopy($array_ptr[$arr_key]));
    }
    // Change array root and loop again
    $array_ptr = &$array_ptr[$arr_key];
  }

  // Deal with appending on the last key
  if ($mode == 'append' && !empty($array_ptr[$last_key]['#default_value'])) {
    // If last key is multiple, add a copy of the existing last key to the end.
    // Because only the item at index zero is editable, we keep the original
    // last key. In effect "prepending" instead of appending.
    if ($last_key_multi) {
      array_unshift($array_ptr, $array_ptr[$last_key]);
    }
    else {
      // It's not a multi-value field. Deal with appending original value with
      // submitted value in text field
      if (!empty($array_ptr[$last_key]['#default_value']) && in_array(array('textarea', 'textfield'), $array_ptr[$last_key]['#type'])) {
        $value['#default_value'] = $array_ptr[$last_key]['#default_value'] . "\n" . $value['#default_value'];
      }
    }
  }

  // Set the value at the last key
  if ($merge && isset($array_ptr[$last_key]) && is_array($array_ptr[$last_key])) {
    $array_ptr[$last_key] = drupal_array_merge_deep($array_ptr[$last_key], (array) $value);
  }
  else {
    $array_ptr[$last_key] = $value;
  }
  dpm($array_ptr);
}
/**
* This function is the clone & replace workhorse. It uses the objective_form_storage to allow new elements to behave like those created direcly using XML values and elements.
* @param array $form
* @param array $form_state
* @param $cloned_elements, previous cloned elements so we don't clone and reclone. 
* @param string $elementspath, the full path(in case of multivalued, theorical) to the target element.
* @param array $input_value , the passed arraay of attributes for the target element
* @param string $mode
* @param string $delimiter
*/
function iwi_set_form_element_byhash(&$form, &$form_state, $cloned_elements, $elementspath, $input_value, $mode = 'replace', $delimiter = ':') {

  $parents = explode($delimiter, $elementspath);
  //XML forms have two special form elements, whose child's need to cloned if $mode=='append'
  // #type in (tags,tabs)
  //So we are going to check for those types before deciding how to handle appends.

  $special_xmlform_elements=array('tags'=>'tag','tabs'=>'tabpanel');

  //We are handling append different, because replace means just setting the value to an existing element. But if the element was already cloned we will have to switch to a replace functionality
  //This means: if we got i.e a submited field like name[0]manePart and then later a name[0]role we can't clone again. So we store the first clone element, and just add the value to this one.Sounds logic?...
  switch($mode) {
    case 'append' :
    $ref = &$form;
    $temp_path = array();
    $key_exists = TRUE;
    $special_element_child_type = NULL;
    $target_element_hash = NULL;
    $child_element_hash=NULL;
    $level=0;
    $last_element=null;
    $special_element_ref = array();
    $previous_partial_path=array();
      foreach ($parents as $parent) {
        if (is_array($ref) && array_key_exists($parent, $ref)) {
          $level++;
          $temp_path[]=$parent;
          if (count($special_element_ref)>0 && $ref[$parent]['#type']==$special_element_child_type)  //Means we already have found an special element container in a previous iteration and we are standing at a child element;
            {
              $future_partial_path=array_slice($parents,$level); //keep track of where we are, store tree descending path
              $previous_partial_path=$temp_path;//Tree up path
              array_pop($previous_partial_path);//Getting rid of the "numeric key" if it's there...
              //Lets get the hash!
              $child_element=&$ref[$parent];
              $child_element_hash = $ref[$parent]['#hash'];
            }
          if (array_key_exists($ref[$parent]['#type'], $special_xmlform_elements)) { //means we are at the root element of a special xml form, we wan't the last one always, in the rare case we have multiple nested ones. 
            $special_element_ref = &$ref[$parent];
            $special_element_child_type = $special_xmlform_elements[$ref[$parent]['#type']];
            //Lets store this point so we know where to append our "cloned" element
            $target_element_hash=$ref[$parent]['#hash'];
            
          }
         
          $ref = &$ref[$parent];
          $last_element = $parent;
        }
        else {
          $key_exists = FALSE; //Means bad things, we passed an invalid path
          //@TODO set some sort of drupal warning
        }
      }
      if ((!empty($target_element_hash)) && (!empty($child_element_hash)))
        { 
          //OK, we got something to clone, let's do it using the objective_form_api:
            //@TODO ask Patrick: if we got no previous values at all for this perticular element, appending means 'replace'?
          $element = $form_state['storage']['objective_form_storage']['root']->findElement($target_element_hash);
          if ($element)
            {
            switch($special_element_child_type)
              {  
              case "tabpanel":
              $tab = $element->findElement($child_element_hash);
              if ($tab) 
                {
                if (!array_key_exists($child_element_hash,$cloned_elements))  //if not already cloned clone it.
                  {
                  $new_tab = clone $tab;
                  $setElementProps = function($element)
                    {
                        $element->default_value = NULL;

                    };
                 $new_tab->eachDecendant($setElementProps);

                $element->adopt($new_tab);
                $new_tab_drupal=$new_tab->toArray(); 

                drupal_array_set_nested_value($new_tab_drupal, array_merge($future_partial_path,array('#attributes')), $input_value['#attributes']);
                drupal_array_set_nested_value($new_tab_drupal, array_merge($future_partial_path,array('#default_value')), $input_value['#default_value']);
              
                $special_element_ref[]=$new_tab_drupal;
             
              //appending the drupal form array back to where it belongs;
                $cloned_elements[$child_element_hash]=end(element_children($special_element_ref)); //Lets store where do we put this one, so we can later just fill out the values if we need so
                  }
              else //Means we already clone the whole container, so just put the values there. We can't use the hash here, we don't know it?
                {
                $fullnewpath=array_merge($previous_partial_path,$cloned_elements[$child_element_hash],$future_partial_path); //This gives us the real path to an previously cloned element.
                //We could also just get the las element_children of the container, and add there...
                drupal_array_set_nested_value($form, array_merge($fullnewpath ,array('#attributes')), $input_value['#attributes']);
                drupal_array_set_nested_value($form, $fullnewpath + array('#default_value'), $input_value['#default_value']);
                }
              }  
              break;
            case "tag" :
            //@TODO ask Patrick: if we got no previous values at all for this perticular element, appending means 'replace'?
            //Now appending at the end. This means the new value is "themed" like a tag. Should, in case of having no previous value, just add the value to the existing input ?([0])..it's like replacing...
            //Still missing a bit. If i add a new value, all new values are the last appended one when using ajax later? Fixit tomorrow.
                $form_children = element_children($special_element_ref);
                $input_field = &$special_element_ref[array_shift($form_children)];
                // Get Input Value as its not stored in the object form.
                $default_value = $input_field['#default_value'];
                $input_field['#value'] = '';
                $element_children = array_values($element->children);
                $input = array_shift($element_children);
                $new_tag = clone $input;
                // Create new tag and have it stored in the state.
                $input->parent->adopt($new_tag);
                $new_tag_drupal =  $new_tag->toArray();
                $new_tag_drupal['#default_value'] =  $input_value['#default_value'];
                // Update drupal form.
                $special_element_ref[]= $new_tag_drupal;  
                break;
            
            }
          }
        
        
        
        }
    break;
    default:
    //We are going to replace existing value.
    //@TODO ask Patrick. In case of multivalues fields. Replace means whipping out every child element of tabs or tags? The thing about replacing in multivalued fields is that we don't know which one to replace. [0]...[n]?
    //If we got only one, it's easy(two lines?), just put our values there.
    $key_exists = NULL;
    $target_element_hash=drupal_array_set_nested_value($new_tab_drupal, array_merge($parents,array('#hash')),$key_exists);
    if ($key_exists)
      {
      drupal_array_set_nested_value($new_tab_drupal, array_merge($parents,array('#attributes')), $input_value['#attributes']);
      drupal_array_set_nested_value($new_tab_drupal, array_merge($parents,array('#default_value')), $input_value['#default_value']);
      $element = $form_state['storage']['objective_form_storage']['root']->findElement($target_element_hash);
      $element->default_value=$input_value['#default_value'];
      //Done.
      }
    }
   

}

function arrayCopy(array $array) {
  $result = array();
  foreach ($array as $key => $val) {
    if (is_array($val)) {
      $result[$key] = arrayCopy($val);
    }
    elseif (is_object($val)) {
      $result[$key] = clone $val;
    }
    else {
      $result[$key] = $val;
    }
  }
  return $result;
}
