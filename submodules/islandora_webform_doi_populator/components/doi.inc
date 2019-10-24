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
    ),
  );
}

function _webform_edit_doi($component) {
  return array();
}

/**
 * Render DOI field
 *
 * @param $component
 * @param null $value
 * @param bool $filter
 *
 * @return array
 */
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
    '#mapping' => array(
      'mode' => 'replace',
      'datastream' => 'MODS (Citation MODS form)',
      'data_stream_id' => 'MODS',
      'field' => 'identifiers:doi'
    )
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
    '#description' => t('The title of this publication'),
    '#mapping' => array(
      'mode' => 'replace',
      'datastream' => 'MODS (Citation MODS form)',
      'data_stream_id' => 'MODS',
      'field' => 'titleInfo:title'
    )
  );
    $form['identifier-uri'] = array(
    '#type' => 'textfield',
    '#title' => t('Identifier: URI'),
    '#attributes' => $isReadOnly,
    '#description' => t('A URI for the item, potentially an HTTP URL'),
    '#default_value' => $filter ? webform_replace_tokens($value['identifier-uri']) : $value['identifier-uri'],
      '#mapping' => array(
        'mode' => 'replace',
        'datastream' => 'MODS (Citation MODS form)',
        'data_stream_id' => 'MODS',
        'field' => 'identifiers:uri'
      )
  );

  $form['identifier-issn'] = array(
    '#type' => 'textfield',
    '#title' => t('Identifier: ISSN'),
    '#attributes' => $isReadOnly,
    '#description' => t('International Standard Serial Number, multiple will be separated by commas'),
    '#default_value' => $filter ? webform_replace_tokens($value['identifier-issn']) : $value['identifier-issn'],
    '#mapping' => array(
      'mode' => 'replace',
      'datastream' => 'MODS (Citation MODS form)',
      'data_stream_id' => 'MODS',
      'field' => 'identifiers:issn'
    )
  );

  $form['dateIssued'] = array(
    '#type' => 'textfield',
    '#title' => t('Date Issued'),
    '#attributes' => $isReadOnly,
    '#default_value' => $filter ? webform_replace_tokens($value['dateIssued']) : $value['dateIssued'],
    '#mapping' => array(
      'mode' => 'replace',
      'datastream' => 'MODS (Citation MODS form)',
      'data_stream_id' => 'MODS',
      'field' => 'originInfo:dateSet:dateIssued'
    )
  );

  $form['pages.start'] = array(
    '#type' => 'textfield',
    '#title' => t('Pages - Start'),
    '#attributes' => $isReadOnly,
    '#default_value' => $filter ? webform_replace_tokens($value['page.start']) : $value['page.start'],
    '#mapping' => array(
      'mode' => 'replace',
      'datastream' => 'MODS (Citation MODS form)',
      'data_stream_id' => 'MODS',
      'field' => 'part:pages:start',
    )
  );


  $form['pages.end'] = array(
    '#type' => 'textfield',
    '#title' => t('Pages - End'),
    '#attributes' => $isReadOnly,
    '#default_value' => $filter ? webform_replace_tokens($value['page.end']) : $value['page.end'],
    '#mapping' => array(
      'mode' => 'replace',
      'datastream' => 'MODS (Citation MODS form)',
      'data_stream_id' => 'MODS',
      'field' => 'part:pages:end',
    )
  );

  $form['volume'] = array(
    '#type' => 'textfield',
    '#title' => t('Volume Number'),
    '#attributes' => $isReadOnly,
    '#default_value' => $filter ? webform_replace_tokens($value['volume']) : $value['volume'],
    '#mapping' => array(
      'mode' => 'replace',
      'datastream' => 'MODS (Citation MODS form)',
      'data_stream_id' => 'MODS',
      'field' => 'part:volume'
    )
  );
  $form['genre'] = array(
    '#type' => 'textfield',
    '#title' => t('Publication Type'),
    '#attributes' => $isReadOnly,
    '#default_value' => $filter ? webform_replace_tokens($value['genre']) : $value['genre'],
    '#mapping' => array(
      'mode' => 'replace',
      'datastream' => 'MODS (Citation MODS form)',
      'data_stream_id' => 'MODS',
      'field' => 'publication_type'
    )
  );

  $form['issue'] = array(
    '#type' => 'textfield',
    '#title' => t('Issue Number'),
    '#attributes' => $isReadOnly,
    '#default_value' => $filter ? webform_replace_tokens($value['issue']) : $value['issue'],
    '#mapping' => array(
      'mode' => 'replace',
      'datastream' => 'MODS (Citation MODS form)',
      'data_stream_id' => 'MODS',
      'field' => 'part:issue'
    )
  );

  $form['author_family_0'] = array(
    '#type' => 'textfield',
    '#title' => t('Authors'),
    '#description' => t('Author\'s family name of the item; Full author names are entered when they appear in the author position of an article, usually on the title page of an article, several authors will be separated by commas.'),
    '#attributes' => $isReadOnly,
    '#default_value' => $filter ? webform_replace_tokens($value['author_family_0']) : $value['author_family_0'],
    '#suffix' =>  '</div></fieldset></div>',
    '#mapping' => array(
      'mode' => 'append',
      'datastream' => 'MODS',
      'data_stream_id' => 'MODS',
      'field' => 'authors:0:family',
      'split_by' => ', '
    )
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

        /**
         * @param $related_items
         * @param string $glue
         *
         * @return string|array
         */
        $getMultiple = function ($related_items, $glue = ', ') use (&$getMultiple) {
          $carry = [];
          foreach ($related_items as $index => $elements) {
            if (!($elements instanceof DOMText) && $elements->hasChildNodes()) {
              $carry[] = $getMultiple($elements->childNodes, $glue);
            }
            else {
              $carry[] = $elements->nodeValue;
            }
          }
          // return contacted list of values
          return (!empty($carry)) ? (($glue) ? implode($glue, $carry) : $carry) : '';
        };

        $getRawValue = function ($query, $position = 0) use ($mods_data) {
          $res = $mods_data->query($query);
          if ($res) {
            return $res[$position]->nodeValue;
          }
          else {
            return NULL;
          }
        };

        $form['genre']['#value'] = $getRawValue('genre');
        $form['titleInfo.title']['#value'] = $getRawValue('titleInfo/title');
        $form['identifier-uri']['#value'] = $getRawValue('identifier[@type="uri"]');
        $form['identifier-issn']['#value'] = $getMultiple($mods_data->query('relatedItem[@type="host"]//identifier[@type="issn"]'));
        $form['dateIssued']['#value'] = $getRawValue('originInfo/dateIssued');
        $form['pages.start']['#value'] = $getRawValue('part/extent[@unit="page"]/start');
        $form['pages.end']['#value'] = $getRawValue('part/extent[@unit="page"]/end');
        $form['volume']['#value'] = $getRawValue('part/detail[@type="volume"]/number');
        $form['issue']['#value'] = $getRawValue('part/detail[@type="issue"]/number');
        $authors = $getMultiple($mods_data->query('name[@type="personal"][descendant::text()[contains(.,"author")]]'), FALSE);

        $all_authors = [];
        $index = 0;

        foreach ($authors as  $single_author) {
          // Remove author key
          $single_author = array_merge(...array_slice($single_author, 0, -1));
          // Set new Xpath to save the data Family, given
          $full_name = $single_author[1] . ' ' . $single_author[0];
          $all_authors[] = $full_name;

        }
        $form["author_family_{$index}"]['#value'] = implode(", ", $all_authors);
        // Close wrapper
        $form["author_family_{$index}"]['#suffix'] = '</div></fieldset></div>';
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
  return $form['submitted']['doi'];
}


function _webform_display_doi($component, $value, $format = 'html') {
  return array(
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#theme' => 'webform_display_textfield',
    '#theme_wrappers' => $format == 'html' ? array('webform_element') : array('webform_element_text'),
    '#post_render' => array('webform_element_wrapper'),
    '#component' => $component,
    '#format' => $format,
    '#field_prefix' => '',
    '#field_suffix' => '',
    '#value' => isset($value[0]) ? $value[0] : '',
  );
}
