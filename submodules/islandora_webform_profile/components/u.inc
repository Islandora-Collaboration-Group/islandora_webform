<?php

function _webform_defaults_u1() {
  return [
    'name' => 'u1',
    'form_key' => 'u1',
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'mandatory' => 1,
    'extra' => [
    ],
  ];
}

function _webform_defaults_u2() {
  return [
    'name' => 'u2',
    'form_key' => 'u2',
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'mandatory' => 1,
    'extra' => [
    ],
  ];
}

function _webform_edit_u1($component) {
  return [];
}

function _webform_edit_u2($component) {
  return _webform_edit_u1($component);
}

function _webform_render_u1($component, $value = NULL, $filter = TRUE) {
  $form = [];
  $form['u1_identifier_tags'] = [
    '#title' => 'Faculty Identifier - U1',
    '#type' => 'textfield',
    '#autocomplete_path' => 'islandora/entities/autocomplete/fp/disambiguated'
  ];
  return $form;
}

function _webform_render_u2($component, $value = NULL, $filter = TRUE) {
  $form = [];
  $form['u2_identifier_tags'] = [
    '#title' => 'Departmental Identifier - U2',
    '#type' => 'textfield',
    '#autocomplete_path' => 'islandora/entities/autocomplete/fp/department'
  ];
  return $form;
}

function _webform_display_u2($component, $value, $format = 'html') {
  return _webform_display_u1($component, $value, $format);
}

function _webform_display_u1($component, $value, $format = 'html') {
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


