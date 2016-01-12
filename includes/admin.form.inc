<?php

/**
 * @file
 * This administration form for this module.
 */

/**
 * The administration form for the islandora webform module.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_webform_admin(array $form, array &$form_state) {
  $form = array(
    'islandora_webform_general' => array(
      '#type' => 'fieldset',
      '#title' => t('Islandora Webform Configuration'),
      'wrapper' => array(
        '#type' => 'markup',
        'islandora_webform_link_target' => array(
          '#type' => 'select',
          '#title' => t('Webform link behavior'),
          '#default_value' => variable_get('islandora_webform_link_target', 'inline'),
          '#description' => t('Select "New tab" if you want a new tab to open for the webform (this allows the user to have the object page and the webform page open in separate windows at the same time); or "Same window" if you want the user to leave the object page to go to the webform page like a standard link.'),
          '#options' => array(
            'new' => "New tab",
            'same' => "Same window",
          ),
        ),
      ),
    ),
  );
  return system_settings_form($form);
}
