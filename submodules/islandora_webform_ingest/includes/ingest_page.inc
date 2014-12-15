<?php

function iwi_ingest_page_form($form, &$form_state, $sid) {

  // Get the submission data
  // Get the submission component mappings
  // Get the datastreams
  // Get forms where possible...
  // .. match up submitted component with field in schema
  // .. insert modified datastream data
  // Otherwise, just preview the content
  // Add submit handler

  $title = t('Ingest webform submission');
  $output = "Some output";


  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit your information'),
  );
  return $form;
}