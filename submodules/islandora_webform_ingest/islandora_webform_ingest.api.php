<?php

/**
 * @file
 * Hooks provided by Islandora Webform Ingest.
 */

/**
 * Allow modules to alter an islandora xml metadata ingest preview form.
 *
 * @param array $form
 *   The drupal form, passed by reference
 *
 * @param array $form_state
 *   The drupal form state
 *
 * @param object $submission
 *   The webform submission record as retrieved from the database
 */
function hook_iwi_set_form_alter(&$form, $form_state, $submission) {
  // Restrict to "Test append and replace" webform
  if ($submission->nid == 14) {
    // Offset is meant to set which tabset to write to.
    $offset = 0;
    $attributes = array(
      'class' => array(
        'submitted-value',
      ),
    );
    // Corporations.
    if (!empty($submission->data[1][0])) {
      $names = array_map('trim', explode(',', $submission->data[1][0]));
      foreach ($names as $name) {
        $input_value = array(
          '#default_value' => 'corporate',
          '#attributes' => $attributes,
        );
        iwi_set_form_element_byhash($form, $form_state, 'nameTabs:namePanel:nameSet:nameType', $input_value, 'append', $offset);
        $input_value = array(
          '#default_value' => $name,
          '#attributes' => $attributes,
        );
        iwi_set_form_element_byhash($form, $form_state, 'nameTabs:namePanel:nameSet:namePart', $input_value, 'append', $offset);
        $offset++;
      }
    }
    // Private individuals.
    if (!empty($submission->data[5][0])) {
      $names = array_map('trim', explode(',', $submission->data[5][0]));
      foreach ($names as $name) {
        $input_value = array(
          '#default_value' => 'personal',
          '#attributes' => $attributes,
        );
        iwi_set_form_element_byhash($form, $form_state, 'nameTabs:namePanel:nameSet:nameType', $input_value, 'append', $offset);
        $input_value = array(
          '#default_value' => $name,
          '#attributes' => $attributes,
        );
        iwi_set_form_element_byhash($form, $form_state, 'nameTabs:namePanel:nameSet:namePart', $input_value, 'append', $offset);
        $offset++;
      }
    }
  }
}


