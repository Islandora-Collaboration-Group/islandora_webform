<?php

function islandora_webform_test_page() {
  module_load_include('inc', 'islandora', 'includes/solution_packs');
  module_load_include('inc', 'islandora', 'includes/utilities');
  module_load_include('inc', 'islandora', 'includes/Object');
  module_load_include('inc', 'islandora_webform', 'includes/utilities');

  $connection = islandora_get_tuque_connection();

  $root_collection = $connection->repository->constructObject('islandora:collectionCModel');
  dpm(islandora_solution_packs_get_required_objects('islandora_basic_image'));
//  $test = module_invoke_all('islandora_viewer_info');
//  $items = module_invoke_all('islandora_required_objects', $connection);
  $cmodels = array();
  $datastreams = array();
  foreach(islandora_solution_packs_get_required_objects() as $name => $item) {
    if(!empty($item['objects'])) {
      foreach($item['objects'] as $object) {
        $cmodels[$name] = $object->models;
        $datastreams[$name] = $object->repository->api->a->listDatastreams($object->id);
      }
    }
  }
//  dpm($cmodels);
//  dpm($datastreams);

  $content_models = islandora_get_content_models();
  foreach($content_models as $content_model_id => $content_model) {
    $content_models[$content_model_id]['datastream_requirements'] = islandora_get_datastreams_requirements_from_models(array($content_model_id));
    $content_models[$content_model_id]['object'] = islandora_object_load($content_model_id);
  }
//  $datastreams = islandora_get_datastreams_requirements_from_models(array_keys($content_models));
  dpm($content_models);
//  dpm($datastreams);

  $enabled_solution_packs = islandora_solution_packs_get_required_objects();
  $output = array();
  foreach ($enabled_solution_packs as $solution_pack_module => $solution_pack_info) {
    $solution_pack_name = $solution_pack_info['title'];
    $objects = array_filter($solution_pack_info['objects']);
    $output[$solution_pack_name] = $solution_pack_info;
  }

  dpm(iw_get_collections_or_content_models('collections'));
  dpm(iw_get_collections_or_content_models('cmodels'));
  dpm($output);

  $object = islandora_object_load('islandora:2');
  dpm($object->relationships->get('info:fedora/fedora-system:def/model#', 'hasModel'));

  dpm($object);
  dpm($object->models);
    return 'Testing';
}

