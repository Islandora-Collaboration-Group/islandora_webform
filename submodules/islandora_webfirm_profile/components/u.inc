<?php

function _webform_defaults_u() {
  return [
    'name' => 'u',
    'form_key' => 'u12',
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'mandatory' => 1,
    'extra' => [
    ],
  ];
}

function _webform_edit_u($component) {
  return [];
}

function _webform_render_u($component, $value = NULL, $filter = TRUE) {
  $form = [];
  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;

  $form['u1_identifier_tags'] = [
    '#title' => 'Faculty Identifier - U1',
    '#type' => 'textfield',
    '#autocomplete_path' => 'islandora/entities/autocomplete/fp/disambiguated',
    '#mapping' => [
      'mode' => 'append',
      'datastream' => 'MODS (Citation MODS form)',
      'data_stream_id' => 'MODS',
      'field' => 'identifiers:u1',
    ]
  ];
  $form['u2_identifier_tags'] = [
    '#title' => 'Departmental Identifier - U2',
    '#type' => 'textfield',
    '#autocomplete_path' => 'islandora/entities/autocomplete/fp/department',
    '#mapping' => [
      'mode' => 'append',
      'datastream' => 'MODS (Citation MODS form)',
      'data_stream_id' => 'MODS',
      'field' => 'identifiers:u2',
    ]
  ];
  return $form;
}

function _webform_display_u($component, $value, $format = 'html') {
  return [
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#theme' => 'webform_display_textfield',
    '#theme_wrappers' => $format == 'html' ? ['webform_element'] : ['webform_element_text'],
    '#post_render' => ['webform_element_wrapper'],
    '#component' => $component,
    '#format' => $format,
    '#field_prefix' => '',
    '#field_suffix' => '',
    '#value' => isset($value[0]) ? $value[0] : '',
  ];
}
