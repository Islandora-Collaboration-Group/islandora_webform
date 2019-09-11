<?php
module_load_include('inc', 'islandora_doi', 'includes/utilities');

function _webform_defaults_doi() {
  return array(
    'name' => 'doi',
    'form_key' => 'doi',
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'mandatory' => 1,
    'extra' => array(
      'field_prefix' => '',
      'field_suffix' => '',
    ),
  );
}

function _webform_edit_doi($component) {
  $form = array();
  $form['field_prefix'] = array(
    '#type' => 'textfield',
    '#title' => t('Left'),
    '#default_value' => $component['extra']['field_prefix'],
    '#size' => 60,
    '#required' => TRUE,
    '#maxlength' => 255,
    '#weight' => 1.1,
    '#parents' => array('extra', 'field_prefix'),
    '#description' => t('Text placed to the left of the slider'),
  );
  $form['field_suffix'] = array(
    '#type' => 'textfield',
    '#title' => t('Right'),
    '#default_value' => $component['extra']['field_suffix'],
    '#size' => 60,
    '#required' => TRUE,
    '#maxlength' => 255,
    '#weight' => 1.2,
    '#parents' => array('extra', 'field_suffix'),
    '#description' => t('Text placed to the right of the slider'),
  );
  return $form;
}

function _webform_render_doi($component, $value = NULL, $filter = TRUE) {
  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;
  $form = array();

  $isReadOnly = array(
    'readonly'=>'readonly',
    'style'=>'background: none repeat scroll 0 0 #EAEAEA;'
  );

  $form['doi'] = array(
    '#type' => 'textfield',
    '#title' => t('Identifier: DOI'),
    '#description' => t('Enter a valid DOI and click on search, example: 10.1038/nphys1170'),
    '#prefix' => '<div id="doi_textfields">',
    '#default_value' => $filter ? webform_replace_tokens($value['doi']) : $value['doi'],
  );
  $form['doi_submit'] = array(
    '#type' => 'button',
    '#value' => t('Search'),
    '#limit_validation_errors' => array(),
    '#ajax' => array(
      'callback' => 'islandora_webform_doi_populator_ajax_callback',
      'wrapper' => 'doi_textfields',
    )
  );

  $form['titleInfo.title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#attributes' => $isReadOnly,
    '#prefix' => '<fieldset class="collapsible form-wrapper collapse-processed" id="doi-metadata"><legend><span class="fieldset-legend">DOI: Metadata</span></legend><div class="fieldset-wrapper"><div class="fieldset-description">Please Review the data below before submitting your form. Data in this section gets pulled automatically from internet.</div>',
    '#default_value' => $filter ? webform_replace_tokens($value['titleInfo.title']) : $value['titleInfo.title'],
    '#description' => t('The title of this publication')
  );
    $form['identifier-uri'] = array(
    '#type' => 'textfield',
    '#title' => t('Identifier: URI'),
    '#attributes' => $isReadOnly,
    '#description' => t('A URI for the item, potentially an HTTP URL'),
    '#default_value' => $filter ? webform_replace_tokens($value['identifier-uri']) : $value['identifier-uri'],
  );

  $form['identifier-issn'] = array(
    '#type' => 'textfield',
    '#title' => t('Identifier: ISSN'),
    '#attributes' => $isReadOnly,
    '#description' => t('International Standard Serial Number, multiple will be separated by commas'),
    '#default_value' => $filter ? webform_replace_tokens($value['identifier-issn']) : $value['identifier-issn'],
  );

  $form['dateIssued'] = array(
    '#type' => 'textfield',
    '#title' => t('Date Issued'),
    '#attributes' => $isReadOnly,
    '#default_value' => $filter ? webform_replace_tokens($value['dateIssued']) : $value['dateIssued'],
  );

  $form['page'] = array(
    '#type' => 'textfield',
    '#title' => t('Pages'),
    '#attributes' => $isReadOnly,
    '#default_value' => $filter ? webform_replace_tokens($value['page']) : $value['page'],
  );
  $form['volume'] = array(
    '#type' => 'textfield',
    '#title' => t('Volume Number'),
    '#attributes' => $isReadOnly,
    '#default_value' => $filter ? webform_replace_tokens($value['volume']) : $value['volume'],
  );
  $form['genre'] = array(
    '#type' => 'textfield',
    '#title' => t('Publication Type'),
    '#attributes' => $isReadOnly,
    '#default_value' => $filter ? webform_replace_tokens($value['genre']) : $value['genre'],
  );

  $form['issue'] = array(
    '#type' => 'textfield',
    '#title' => t('Issue Number'),
    '#attributes' => $isReadOnly,
    '#default_value' => $filter ? webform_replace_tokens($value['issue']) : $value['issue'],
  );
  $form['authors'] = array(
    '#type' => 'textfield',
    '#title' => t('Authors'),
    '#description' => t('Authors of the item; Full author names are entered when they appear in the author position of an article, usually on the title page of an article, several authors will be separated by commas.'),
    '#attributes' => $isReadOnly,
    '#default_value' => $filter ? webform_replace_tokens($value['authors']) : $value['authors'],
    '#suffix' => '</div></fieldset></div>'
  );
  //get cid
  if (isset($_POST['submitted']) && isset($_POST['submitted'][$component['form_key']])) {
    $_doi_value = $_POST['submitted'][$component['form_key']][$component['type']];
    if (!empty($_doi_value) ) {
      $mods = islandora_doi_get_mods($_doi_value);
      if ($mods === FALSE) {
        form_error($element, t('"@value" does not appear to be a DOI we can handle.', array(
          '@value' => $element['#value'],
        )));
      } else {
        // MODS is valid parse and return
        $mods_data = new DOMXPath($mods);
        $concatenate_values = function ($elements) {
          $carry = '';
          foreach ($elements as $item) {
            $carry = string_conca
          }

        };

        $form['genre']['#value'] = $mods_data->query('genre')[0]->nodeValue;
        $form['titleInfo.title']['#value'] = $mods_data->query('relatedItem/titleInfo/title')[0]->nodeValue;
        $form['identifier-uri']['#value'] = $mods_data->query('identifier[@type="uri"]')[0]->nodeValue;
        $form['identifier-issn']['#value'] = array_reduce( $mods_data->query('relatedItem[@type="host"]//identifier[@type="issn"]'), 'concatenate_values');
      }

    }
  }
  return $form;
}

/**
 * Callback for the DOI AJAX lookup callback.
 *
 *
 * @return array
 *   Renderable array (the textfields element)
 */

function islandora_webform_doi_populator_ajax_callback($form, $form_state) {
  return $form['submitted']['doi2'];
}


function _webform_display_doi($component, $value, $format = 'html') {
  return array(
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#theme' => 'webform_display_textfield',
    '#theme_wrappers' => $format == 'html' ? array('webform_element') : array('webform_element_text'),
    '#post_render' => array('webform_element_wrapper'),
    '#field_prefix' => $component['extra']['field_prefix'],
    '#field_suffix' => $component['extra']['field_suffix'],
    '#component' => $component,
    '#format' => $format,
    '#value' => isset($value[0]) ? $value[0] : '',
  );
}
