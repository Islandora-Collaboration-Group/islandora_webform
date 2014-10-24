<?php

/**
 * @file
 * Handles islandora-specific configuration for webforms.
 */

/**
 * Create a islandora configuration page for webforms
 *
 * @param $form
 * @param $form_state
 * @param $node
 * @return mixed
 */
function islandora_webform_configure_form($form, &$form_state, $node) {

  $form['#submit'] = array(
    'islandora_webform_configure_form_submit',
  );


  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );

  $form_state['create_islandora_webform_data'] = FALSE;
  if(!empty($node->nid)) {
    $defaults = db_select('islandora_webform_webforms','w')
      ->fields('w')
      ->condition('w.entity_id', $node->nid)
      ->execute()->fetchAssoc();
  }
  if(empty($defaults)) {
    $form_state['create_islandora_webform_data'] = TRUE;

    $defaults = array(
      'enabled' => variable_get('islandora_webform_default_enabled', 1),
      'pid_filter' => variable_get('islandora_webform_default_pid_filter', ''),
      'scope' => variable_get('islandora_webform_default_scope', 'tagged'),
      'link_text' => variable_get('islandora_webform_default_link_text', 'Go to web form'),
      'link_help' => variable_get('islandora_webform_default_link_help', 'Your input is requested!'),
    );
  }
  $form['islandora'] = array(
    '#type' => 'fieldset',
    '#title' => 'Islandora Options',
    '#collapsible' => FALSE,
    '#group' => 'additional_settings',
    'enabled' => array(
      '#type' => 'checkbox',
      '#title' => 'Enabled',
      '#description' => t('Check to permit this webform to be linked from Islandora objects'),
      '#default_value' => $defaults['enabled'],
    ),
    'pid_filter' => array(
      '#type' => 'textfield',
      '#title' => 'PID Search String',
      '#description' => t('Enter a regex pattern to match against Fedora object PIDs (optional)'),
      '#maxlength' => 200,
      '#default_value' => $defaults['pid_filter'],
    ),
    'scope' => array(
      '#type' => 'select',
      '#title' => t('Add a link to...'),
      '#description' => t(''),
      '#default_value' => $defaults['scope'],
      '#options' => array(
        'all' => 'All matched objectives',
        'tagged' => 'Only those objectives that are manually tagged for this webform',
      ),
    ),
    'link_text' => array(
      '#type' => 'textfield',
      '#title' => 'Link text',
      '#description' => t('The text that will appear in the link to this webform'),
      '#maxlength' => 60,
      '#default_value' => $defaults['link_text'],
    ),
    'link_help' => array(
      '#type' => 'textfield',
      '#title' => 'Link help text',
      '#description' => t('The text that will appear as help/description text for the link'),
      '#maxlength' => 60,
      '#default_value' => $defaults['link_help'],
    ),
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Submit handler for islandora webform configuration page
 *
 * @param $node
 * @param $form_state
 */
function islandora_webform_configure_form_submit($node, $form_state) {

  $nid = $node['nid']['#value'];
  _islandora_webform_insert_pid_component($nid);
  if (db_merge('islandora_webform_webforms')
    ->key(array('entity_id' => $nid))
    ->fields(array(
      'enabled' => $form_state['values']['enabled'],
      'pid_filter' => $form_state['values']['pid_filter'],
      'scope' => $form_state['values']['scope'],
      'link_text' => $form_state['values']['link_text'],
      'link_help' => $form_state['values']['link_help'],
    ))
    ->execute()) {
    drupal_set_message(t('Islandora configuration saved.'));
  }
  else {
    drupal_set_message(t('Islandora configuration could not be saved.'));
  }
}


/**
 * Adds an islandora_object_pid textfield to a webform if one does not
 * already exist.
 *
 * @param $nid
 * @throws Exception
 */
function _islandora_webform_insert_pid_component($nid) {

  $webform = node_load($nid);
  if($webform) {
    $component = FALSE;

    // Look to see if the component already exists, if so, then do nothing
    if (!empty($webform->webform['components'])) {
      foreach($webform->webform['components'] as $test_component) {
        if($test_component['form_key'] == 'islandora_object_pid') {
          $component = $test_component;
          break;
        }
      }
    }

    // If no component found, then add it and save
    if(!$component) {
      $component = array(
        'name' => 'Islandora object PID',
        'form_key' => 'islandora_object_pid',
        'type' => 'textfield',
        'mandatory' => 0,
        'weight' => 15,
        'extra' => array(
          'title_display' => 'inline',
          'private' => 0,
        )
      );
      webform_component_defaults($component);

      $webform->webform['components'][] = $component;
      node_save($webform);
      dpm($webform);
    }
  }
}