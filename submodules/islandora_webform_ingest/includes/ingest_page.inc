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
    '#datastreams' => $datastreams,
    '#submission' => $submission,
  );


  // Build array of ingest datastreams, each with its component mappings
  $ingest_datastreams = array();

  // Loop through the component mappings, adding submitted values to appropriate datastream sub-forms
  foreach ($mappings as $mapping) {
    // Check if we have submitted data for this component, if not, skip it entirely
    if (isset($submission->data[$mapping->cid][0])) {
      $submitted_value = $submission->data[$mapping->cid][0];

      $component_info = $webform->webform['components'][$mapping->cid];
      $dsid = $mapping->data['datastream_id'];
      $dsindex = $mapping->data['datastream'];
      $association = $datastreams[$dsindex];

      // If we haven't mapped anything on this datastream yet, we need to add the
      // datastream sub-form as a fieldset to the aggregate form.
      if (empty($aggregate_form[$dsid][$dsid])) {

        $new_form = array(
          '#title' => isset($association['form_name']) ? $association['form_name'] : $dsid,
          '#type' => 'fieldset',
          '#collapsible' => FALSE,

        );

        // If datastream has an edit form, add that to the aggregated form
        if (isset($association['form_name'])) {

          // Build the form array from schema...
          $new_form_state = array();

          $new_form_state['association'] = $association;
          $new_form_name = $association['form_name'];
          $xml = $object[$dsid]->content;

          $new_form_tmp = array();
          // This is where the magic happens and all xml_form components are initialized.
          $new_form_tmp = xml_form_builder_get_form($new_form_tmp, $new_form_state, $new_form_name, $xml);


          $new_form[$dsid] = $new_form_tmp;

          $new_form[$dsid]['pre_form_state'] = array(
            '#type' => 'value',
            '#value' => $new_form_state
          );

          //Diego:I think you already managed this, but just in case.
          $title_field_parents = $association['title_field'];
          $title_field_parents[] = '#required';
          // TODO: where is $title_field supposed to be used?
          $title_field = drupal_array_set_nested_value($new_form[$dsid], $title_field_parents, TRUE);

          $ingest_datastreams[$dsindex]['form'] = $form;

        }
        else {
          // Show a simple input element inside the sub-form
          $new_form[$dsid] = array(
            '#type' => $component_info['type'],
            // Since this datastream form only has a single input element
            // we can set the default value here when we create the form.
            '#default_value' => $submission->data[$mapping->cid][0],
            '#attributes' => array(
              'class' => array(
                'submitted-value'
              )
            )
          );
        }

        // Merge the sub-form into aggregate
        $aggregate_form[$dsid] = $new_form;
      }
      // Done building the form...


      // If this is an xml form, insert the submitted value on
      // the correct input element
      if (!empty($aggregate_form[$dsid][$dsid]) && isset($association['form_name'])) {
        $field_submitted_value = array(
          '#default_value' => $submitted_value,
          '#attributes' => array(
            'class' => array(
              'submitted-value'
            )
          )
        );
        // Insert the submitted value into the form
        // TODO: Deal with mode (append, replace) and multi-value fields here?
        iwi_array_path_set_value($aggregate_form[$dsid][$dsid], $mapping->data['field'], $field_submitted_value);
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

    $object = $form['#object'];

    foreach ($form['#datastreams'] as $association) {
      $dsid = $association['dsid'];
      if (isset($form_state['values'][$dsid]) && isset($form[$dsid][$dsid])) {

        // Importing via xml form?
        if (isset($association['form_name'])) {

          // Pull out the sub form from the aggregate form
          $sub_form = $form[$dsid][$dsid];

          // Reload the original sub_form $form_state
          $new_form_state = $form_state['values'][$dsid]['pre_form_state'];

          // Unset pre_form_state from the main $form_state: We don't need
          // nor want a form_state inside a form_state!
          unset($form_state['values'][$dsid]['pre_form_state']);

          // Load the input/values to our new form state
          $new_form_state['input'] = $form_state['input'][$dsid];
          $new_form_state['values'] = $form_state['values'][$dsid];

          // Create XMLForm object from new form state
          $xml_form = new XMLForm($new_form_state);

          // Process the form
          $document = $xml_form->submit($sub_form, $new_form_state);

          $title_field = drupal_array_get_nested_value($sub_form, $association['title_field']);
          $label = $title_field['#value'];

          // Enable to preview the XML after the submission was done...
          //dpm(dom_document_pretty_print($document->document));

          // Update the islandora object with this datastream
          xml_form_builder_update_object($form['#object'], $association, $document, $label);
        }
        // Done handling form-based datastream

        // Handle other datastream mimetypes
        else {
          // TODO: Do we need to itemize each mime type?
          // Get the datastream object that we'll be ingesting into
          $datastream = isset($object[$dsid]) ? $object[$dsid] : $object->constructDatastream($dsid);
          switch ($association['mime']) {
            case 'text/plain':
              $datastream->content = $form_state['values'][$dsid];
              $object->ingestDatastream($datastream);
              break;
            case 'application/pdf':
            case 'image/jpeg':
              $file = file_load($form_state['values'][$dsid]);
              $file_path = drupal_realpath($file->uri);
              $datastream->setContentFromFile($file_path, FALSE);
              $datastream->label = $file->filename;
              $datastream->mimetype = $file->filemime;
              $object->ingestDatastream($datastream);
              break;
            default:
              drupal_set_message(t('%mime is not a supported mime type for ingest', array('%mime' => $association['mime']), 'error'));
              break;
          }
        }
      }
    }
    iwi_record_ingested($form['#submission'], $object);
  }
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