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
    $attributes = array(
      'class' => array(
        'submitted-value',
      ),
    );

    // Get a list of previously added nameTabs so we can remove them later.
    $old_name_tabs = element_children($form['nameTabs']);

    // Corporations.
    if (!empty($submission->data[1][0])) {
      $names = array_map('trim', explode(',', $submission->data[1][0]));
      foreach ($names as $name) {
        $offset = md5(uniqid('offset', TRUE));
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
      }
    }
    // Private individuals.
    if (!empty($submission->data[5][0])) {
      $names = array_map('trim', explode(',', $submission->data[5][0]));
      foreach ($names as $name) {
        $offset = md5(uniqid('offset', TRUE));
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
      }
    }
    // Remove the original nameTabs that we saved above.
    foreach ($old_name_tabs as $old_name_tab) {
      // Get the objective form hash from the drupal form element.
      $hash = $form['nameTabs'][$old_name_tab]['#hash'];
      // Find that element in the objective form storage and orphan (remove) it.
      // We have to do this so that ajax won't reload those original tabs when the user
      // adds a new tab.
      $form_state['storage']['objective_form_storage']['root']->findElement($hash)->orphan();
      // Finally, we can safely delete the drupal form element.
      unset($form['nameTabs'][$old_name_tab]);
    }
  }
}

