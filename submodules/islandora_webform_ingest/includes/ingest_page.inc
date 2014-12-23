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

            // Need to set these values in form_state for the xml_form_builder_get_form function
            $new_form_state = array(
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
            );

            $xml = $new_form_state['datastream']->content;
            $form_name = $association['form_name'];
//            unset($form_state['storage'][FormStorage::STORAGE_ROOT]);

            $new_form[$association['dsid']] = xml_form_builder_get_form($new_form, $new_form_state, $form_name, $xml);

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

//function xml_form_builder_edit_datastream_form_metadata_form_submit(array $form, array &$form_state) {
//  $datastream = $form_state['datastream'];
//  $association = $form_state['association'];
//  $object = islandora_object_load($datastream->parent->id);
//
//  $title_field = drupal_array_get_nested_value($form, $association['title_field']);
//  $label = $title_field['#value'];
//
//  $xml_form = new XMLForm($form_state);
//  $document = $xml_form->submit($form, $form_state);
//  dpm(get_defined_vars());
//  xml_form_builder_update_object($object, $association, $document, $label);
//  $form_state['redirect'] = "islandora/object/{$object->id}/manage";
//}

/**
 * Form-submit callback for islandora webform ingest submission page
 *
 * @param $form
 * @param $form_state
 */
function iwi_ingest_page_form_submit($form, &$form_state) {
  if (!empty($form['#datastreams']) && !empty($form['#object'])) {
    $object = $form['#object'];
    foreach ($form['#datastreams'] as $dsid => $association) {
      if (isset($form_state['values'][$dsid]) && isset($form[$dsid][$dsid])) {

        // Get the datastream object that we'll be ingesting into
        $datastream = isset($object[$dsid]) ? $object[$dsid] : $object->constructDatastream($dsid);

        // Handle xml forms
        if (isset($association['form_name'])) {
          $sub_form = $form[$dsid][$dsid];
          $xml_form = new XMLForm($sub_form);
          $new_form_state = $form_state;
          $new_form_state['input'] = $form_state['input'][$dsid];
          $new_form_state['values'] = $form_state['values'][$dsid];
          if(!empty($sub_form['storage'][FormStorage::STORAGE_ROOT])) {
            $new_form_state['storage'][FormStorage::STORAGE_ROOT] = $sub_form['storage'][FormStorage::STORAGE_ROOT];
            unset($sub_form['storage'][FormStorage::STORAGE_ROOT]);
          }
          dqdpm(get_defined_vars());
          $document = $xml_form->submit($sub_form, $new_form_state);
//      xml_form_builder_update_object($form['#object'], $association, $document, $label);
        }
        else {
          // Handle other mimetypes
          switch($association['mime']) {
            case 'text/plain':
              $datastream->content = $form_state['values'][$dsid];
              $object->ingestDatastream($datastream);
              break;
          }
        }
      }
    }
  }
  dqdpm(get_defined_vars());
}