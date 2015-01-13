<?php


/**
 * Callback to build islandora webform ingest submission page.
 *
 * @param array $form
 *   form
 * @param array $form_state
 *   form_state
 * @param int $sid
 *   the submission ID
 *
 * @return array
 *   A renderable Drupal form array
 *
 * @throws Exception
 */
function iwi_preview_ingest_submission_form($form, &$form_state, $sid) {
  module_load_include('inc', 'webform', 'includes/webform.submissions');
  module_load_include('inc', 'islandora_webform', 'includes/utilities');
  module_load_include('inc', 'islandora_webform_ingest', 'includes/utilities');
  drupal_add_css(drupal_get_path('module', 'islandora_webform_ingest') . '/css/islandora_webform_ingest.css');
  form_load_include($form_state, 'inc', 'islandora_webform_ingest', '/includes/ingest_page');
  form_load_include($form_state, 'inc', 'islandora', '/includes/ingest.form');
  // Get the submission data.
  $submission = array_shift(webform_get_submissions(array('sid' => $sid)));
  $object = $submission->islandora_object;
  $webform = node_load($submission->nid);
  // Get the submission component mappings.
  $mappings = iwi_get_saved_component_mappings($submission->nid);

  // Get the content model datastreams.
  $datastreams = iwi_ingestable_datastreams(islandora_object_load(iwi_webform_content_model($submission->nid)));

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
  $form['submission_info'] = array(
    '#type' => 'fieldset',
    '#title' => t('Submission info'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => -1,
    'description' => array(
      '#markup' => theme('table', array(
        'rows' => array($submission_info),
        'header' => $submission_header,
      )),
    ),
    'submission' => webform_submission_render($webform, $submission, NULL, NULL),
  );
  $form_state['object'] = $object;
  $form_state['datastreams'] = $datastreams;
  $islandora_webform_values = array();

  // We are going to store the already cloned elements to avoid multiple
  // new form elements with partial equal paths to generate multiple children.
  // (like a tabpanel) e.g if we have "name:0:namePart" and "name:0:role"
  // ...this could give to additional tab panels being added, even when both
  // belong to only one. Used in iwi_set_form_element_byhash, utilities.inc
  $cloned_elements = array();

  // Loop through the component mappings, adding submitted values to
  // appropriate datastream sub-forms
  foreach ($mappings as $mapping) {
    // Check if we have submitted data for this component, if not, skip it
    // entirely.
    if (isset($submission->data[$mapping->cid][0])) {
      $submitted_value = $submission->data[$mapping->cid][0];
      $dsid = $mapping->data['datastream_id'];
      $dsindex = $mapping->data['datastream'];
      $association = $datastreams[$dsindex];

      // Store all our submited values.
      $islandora_webform_values[$dsid]['association'] = $association;
      $islandora_webform_values[$dsid]['data_values'][] = array(
        'path' => $mapping->data['field'],
        'input_value' => array(
          '#default_value' => $submitted_value,
          '#attributes' => array(
            'class' => array(
              'submitted-value',
            ),
          ),
        ),
        'mode' => $mapping->data['mode'],
      );
    }
  }
  // We need to preserve the original $form_state to pass it to
  // xml_form_builder_get_form.
  // On the first build pass.
  $form_state_original = $form_state;

  foreach ($islandora_webform_values as $dsid => $dsid_values) {
    // If datastream has an edit form, add that to the form.
    if (isset($dsid_values['association']['form_name'])) {
      $new_form_name = $dsid_values['association']['form_name'];
      $xml = NULL;
      if (isset($object[$dsid])) {
        $xml = $object[$dsid]->content;
      }
      $form2 = NULL;
      // Deal with ajax. We are going to simulate a multistep form using only
      // one dsid at a time. Dirty!
      if (isset($form_state['rebuild']) && $form_state['rebuild'] == TRUE) {
        $form_state_new['values'] = $form_state['values'][$dsid];
        $form_state_new['storage']['objective_form_storage'] = $form_state['storage']['islandora_webform'][$dsid];
        $form_state_new['triggering_element'] = $form_state['triggering_element'];
        $form2 = xml_form_builder_get_form(array(), $form_state_new, $new_form_name, $xml);
      }
      // End deal with ajax.
      else {
        $form_state_new = $form_state_original;
        $form2 = xml_form_builder_get_form($form, $form_state_new, $new_form_name, $xml);
      }

      // Only do this once. If ajax is called, then the form is rebuilt and
      // we end re-adding new elements. So check if form was rebuild. If yes,
      // don't do anything.
      if (!isset($form_state['rebuild']) || $form_state['rebuild'] == FALSE) {
        // Loop through the submitted values, setting them into $form2.
        foreach ($dsid_values['data_values'] as $input) {
          iwi_set_form_element_byhash($form2, $form_state_new, $cloned_elements, $input['path'], $input['input_value'], $input['mode']);
        }
      }
      // Build our own local and multiple objective_form_storage for every dsid.
      $form_state['storage']['islandora_webform'][$dsid] = $form_state_new['storage']['objective_form_storage'];

      $new_form = array(
        '#title' => isset($dsid_values['association']['form_name']) ? $dsid_values['association']['form_name'] : $dsid,
        '#type' => 'fieldset',
        '#tree' => TRUE,
        '#collapsible' => FALSE,
      );
      foreach (element_children($form2) as $childkey) {
        $new_form[$childkey] = $form2[$childkey];
      }
      // Recover the #hash.
      $new_form['#hash'] = $form2['#hash'];


      $form[$dsid] = $new_form;
    }
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Ingest this Submission'),
    '#name' => 'submit',
    '#submit' => array('iwi_ingest_page_form_submit'),
    '#weight' => 100,
  );

  $form['#submission'] = $submission;

  return $form;

}

/**
 * Form-submit callback for islandora webform ingest submission page.
 *
 * @param array $form
 *   the form
 * @param array $form_state
 *   the form_state
 */
function iwi_ingest_page_form_submit($form, &$form_state) {
  if (!empty($form_state['datastreams']) && !empty($form_state['object'])) {

    $object = $form_state['object'];

    foreach ($form_state['datastreams'] as $association) {
      $dsid = $association['dsid'];
      if (isset($form_state['values'][$dsid]) && isset($form[$dsid])) {

        // Importing via xml form?
        if (isset($association['form_name'])) {

          // Pull out the sub form from the aggregate form.
          $sub_form = $form[$dsid];

          // Reload the original sub_form $form_state
          $new_form_state['storage']['objective_form_storage'] = $form_state['storage']['islandora_webform'][$dsid];


          // Load the input/values to our new form state.
          $new_form_state['input'] = $form_state['input'][$dsid];
          $new_form_state['values'] = $form_state['values'][$dsid];
          // Create XMLForm object from new form state.
          $xml_form = new XMLForm($new_form_state);

          // Process the form.
          $document = $xml_form->submit($sub_form, $new_form_state);

          // Enable to preview the XML after the submission was done...
          // dpm(dom_document_pretty_print($document->document));

          // Get updated object label.
          $label = NULL;
          if (is_array($association['title_field'])) {
            $title_field = drupal_array_get_nested_value($sub_form, $association['title_field']);
            $label = $title_field['#value'];
          }

          // Update the islandora object with this datastream.
          xml_form_builder_update_object($object, $association, $document, $label);
        }
        // Done handling form-based datastream.

        // Handle other datastream mimetypes.
        else {
          // TODO: Do we need to itemize each mime type?
          // Get the datastream object that we'll be ingesting into.
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
 * Save a record of an islandora webform ingestion.
 *
 * @param object $submission
 *   A webform submission object
 * @param IslandoraFedoraObject $object
 *   The repository object
 *
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

  db_insert('islandora_webform_ingestions')->fields($fields)->execute();
}