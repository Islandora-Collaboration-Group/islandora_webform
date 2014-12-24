<?php


/**
 * Callback to build islandora webform ingest submission page
 *
 * @param $form
 * @param $form_state
 * @param $sid
 *  the submission ID
 *
 * @return array
 * @throws Exception
 */
function iwi_ingest_page_form($form, &$form_state, $sid) {

  module_load_include('inc', 'webform', 'includes/webform.submissions');
  module_load_include('inc', 'islandora_webform', 'includes/utilities');
  module_load_include('inc', 'islandora_webform_ingest', 'includes/utilities');
  drupal_add_css(drupal_get_path('module', 'islandora_webform_ingest') . '/css/islandora_webform_ingest.css');
  form_load_include($form_state, 'inc', 'islandora_webform_ingest', '/includes/ingest_page');
  form_load_include($form_state, 'inc', 'islandora', '/includes/ingest.form');

  // Get the submission data
  $submission = array_shift(webform_get_submissions(array('sid' => $sid)));
  $webform = node_load($submission->nid);
  $object = $submission->islandora_object;

  // Get the submission component mappings
  $mappings = iwi_get_saved_component_mappings($submission->nid);

  // Get the content model datastreams
  $datastreams = iwi_ingestable_datastreams(islandora_object_load(iwi_webform_content_model($submission->nid)));

  // Create submission information collapsed fieldset and put at top of form
  $submission_info = array(
    l($object->label, 'islandora/object/' . $object->id),
    l($webform->title, 'node/' . $webform->nid),
    l($submission->is_draft ? t('@sid (draft)', array('@sid' => $sid)) : $sid, "node/$webform->nid/submission/$sid"),
    format_date($submission->submitted, 'short'),
    theme('username', array('account' => $submission)),
  );
  $submission_header = array(
    t('Object'),
    t('Webform'),
    t('Submission'),
    t('Submitted on'),
    t('Submitted by'),
  );
  $aggregate_form = array(
    'submission_info' => array(
      '#type' => 'fieldset',
      '#title' => t('Submission info'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#weight' => -1,
      'description' => array(
        '#markup' => theme('table', array(
          'rows' => array($submission_info),
          'header' => $submission_header
        )),
      ),
      'submission' => webform_submission_render($webform, $submission, NULL, NULL),
    ),
    '#object' => $object,
  );


  // Build array of ingest datastreams, each with its component mappings
  $ingest_datastreams = array();
  foreach ($mappings as $mapping) {
    // Check if we have submitted data for this component
    if (isset($submission->data[$mapping->cid][0])) {
      $submitted_value = $submission->data[$mapping->cid][0];

      $component_info = $webform->webform['components'][$mapping->cid];

      if (empty($ingest_datastreams[$mapping->data['datastream']])) {
        $dsindex = $mapping->data['datastream'];

        if (!empty($datastreams[$dsindex])) {

          $association = $datastreams[$dsindex];

          $new_form = array(
            '#title' => isset($association['form_name']) ? $association['form_name'] : $association['dsid'],
            '#type' => 'fieldset',
            '#collapsible' => FALSE,

          );
          
          // If datastream has an edit form, add that to the aggregated form
          if (isset($association['form_name'])) {
			  $new_form_state=array();
			 //Diego:We don't need a full form_state here, just the enough data so we get a form_state['storage'] back from xml_form_builder_get_form. Passing this to the next handler avoids recreating the XML form from scratch.
          
		    // Need to set these values in form_state for the xml_form_builder_get_form function
           
		    /*$new_form_state = array(
              'build_info' => array(
                'args' => array(
                  0 => $object[$association['dsid']]
                ),
                'form_id' => isset($form_state['build_info']['form_id']) ? $form_state['build_info']['form_id'] : 'iwi_preview_ingest_submission_form',
              ),
              'rebuild' => isset($form_state['rebuild']) ? $form_state['rebuild'] : FALSE,
              'rebuild_info' => array(),
              'redirect' => NULL,
              'temporary' => array(),
              'submitted' => FALSE,
              'executed' => FALSE,
              'programmed' => FALSE,
              'programmed_bypass_access_check' => TRUE,
              'cache' => FALSE,
              'method' => 'post',
              'groups' => array(),
              'buttons' => array(),
              'input' => array(),
              'datastream' => $object[$association['dsid']],
              'association' => $association,
            );*/
			
            //$xml = $new_form_state['datastream']->content;
            //$form_name = $association['form_name'];
           //unset($form_state['storage'][FormStorage::STORAGE_ROOT]);
		 
		 //Diego: No need for passing the datastream around.
		 //Why? Tuque is tricky. If we assign a datastream to this form again, then Tuque want's to edit the datastream version! And we already have the object. So the target datastream can be derived from association.
		
             //$new_form_state['datastream']=$object[$association['dsid']];
			 $new_form_state['association']=$association;
			 $new_form_name=$association['form_name'];
		     $xml =  $object[$association['dsid']]->content;
			 
			 $new_form_tmp=array();
			 $new_form_tmp = xml_form_builder_get_form($new_form_tmp, $new_form_state, $new_form_name, $xml); //This is where the magic happens and all xml_form components are initialized,
		    
			 
			 $new_form[$association['dsid']]=$new_form_tmp;
			 
			
			 dpm($new_form_state);
			 $new_form[$association['dsid']]['pre_form_state']= array(
    			 '#type' => 'value',
   			     '#value' => $new_form_state
		  );
			 
		     //Diego:I think you already managed this, but just in case.
  		     $title_field_parents = $association['title_field'];
  		     $title_field_parents[] = '#required';
  		     $title_field = drupal_array_set_nested_value($new_form[$association['dsid']], $title_field_parents, TRUE);
			 
		   
		   
		  
		 
		   
		 
			
			$field_submitted_value = array(
              '#default_value' => $submitted_value,
              '#attributes' => array(
                'class' => array(
                  'submitted-value'
                )
              )
            );
            // Fill in the submitted value in the form
            iwi_array_path_set_value($new_form[$association['dsid']], $mapping->data['field'], $field_submitted_value);


            $ingest_datastreams[$dsindex]['form'] = $form;

          }
          else {
            // Show a simple input element
            $new_form[$association['dsid']] = array(
              '#type' => $component_info['type'],
              '#default_value' => $submission->data[$mapping->cid][0],
              '#attributes' => array(
                'class' => array(
                  'submitted-value'
                )
              )
            );
          }


          $ingest_datastreams[$dsindex]['datastream'] = $datastreams[$dsindex];

          // Merge into aggregate
          $aggregate_form[$association['dsid']] = $new_form;

          // Keep a list of which datastreams we are ingesting
          $aggregate_form['#datastreams'][$association['dsid']] = $association;
        }
      }

      $ingest_datastreams[$mapping->data['datastream']]['mappings'][] = $mapping;
    }
  }
   
  // Add submit handler
  $aggregate_form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Ingest this Submission'),
    '#name' => 'submit',
    '#submit' => array('iwi_ingest_page_form_submit'),
    '#weight' => 100,
  );

  return $aggregate_form;
}

/**
 * Form-submit callback for islandora webform ingest submission page
 *
 * @param $form
 * @param $form_state
 */
function iwi_ingest_page_form_submit($form, &$form_state) {
  if (!empty($form['#datastreams']) && !empty($form['#object'])) {
    foreach ($form['#datastreams'] as $dsid => $association) {
      if (isset($form_state['values'][$dsid]) && isset($form[$dsid][$dsid])) {
        if (isset($association['form_name'])) {

          // Importing through xml form
          $sub_form = $form[$dsid][$dsid];
		  $new_form_state=$form_state['values'][$dsid]['pre_form_state']; //Diego: reloading the original sub_form $form_state;
		  unset($form_state['values'][$dsid]['pre_form_state']); //Diego: unsetting now pre_form_state from the main $form_state values: That was just a temporary hack. We don't need nor wan't a form_state inside a form_state!
          $new_form_state['input'] = $form_state['input'][$dsid];
          $new_form_state['values'] = $form_state['values'][$dsid];
          dpm($new_form_state);
		  $xml_form = new XMLForm($new_form_state);
		  
		  
          $document = $xml_form->submit($sub_form, $new_form_state);
		  $title_field= drupal_array_get_nested_value($sub_form , $association['title_field']);
  		  $label = $title_field['#value'];
		   // Diego: enable if you preview the XML after the submission was done
		   //dpm(dom_document_pretty_print($document->document));
          xml_form_builder_update_object($form['#object'], $association, $document,$label);		
        }
        else {
          // Not importing through xml form
          // TODO
         // dqdpm(get_defined_vars());

        }
      }
    }
    iwi_record_ingested($form['#submission'], $object);
  }
  //dqdpm(get_defined_vars());
}

/**
 * Save a record of an islandora webform ingestion
 *
 * @param $submission
 * @param IslandoraFedoraObject $object
 * @throws Exception
 */
function iwi_record_ingested($submission, IslandoraFedoraObject $object) {
  $fields = array(
    'nid' => $submission->nid,
    'sid' => $submission->sid,
    'pid' => $object->id,
    'date' => REQUEST_TIME,
    'data' => serialize($submission->data),
  );

  db_insert('islandora_webform_ingestions')->fields($fields);
}