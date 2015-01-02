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
        '#prefix' => '<div id="islandora-webform-link-inject">',
        '#suffix' => '</div>',
        '#type' => 'markup',
        'islandora_webform_inject_link' => array(
          '#type' => 'checkbox',
          '#title' => t('Inject webform link'),
          '#default_value' => variable_get('islandora_webform_inject_link', 1),
          '#description' => t('Uncheck to use your own template files to add the webform link on islandora solution pack object display pages. Check this box to insert the links immediately after the table of metadata.'),
          '#required' => FALSE,
        ),
        'islandora_webform_link_target_new_page' => array(
          '#type' => 'checkbox',
          '#title' => t('Open webforms in new window'),
          '#default_value' => variable_get('islandora_webform_link_target_new_page', 1),
          '#description' => t('Check if you want islandora webform links to open in a new page. This allows the user to have the object page and the webform page open at the same time.'),
          '#required' => FALSE,
        ),
      ),
    ),
  );
  return system_settings_form($form);
}
