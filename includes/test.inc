<?php

function islandora_webform_test_page() {
  module_load_include('inc', 'islandora', 'includes/solution_packs');
  module_load_include('inc', 'islandora', 'includes/utilities');
  module_load_include('inc', 'islandora', 'includes/Object');

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
  dpm($cmodels);
  dpm($datastreams);

  $enabled_solution_packs = islandora_solution_packs_get_required_objects();
  $output = array();
  foreach ($enabled_solution_packs as $solution_pack_module => $solution_pack_info) {
    $solution_pack_name = $solution_pack_info['title'];
    $objects = array_filter($solution_pack_info['objects']);
    $output[$solution_pack_name] = $solution_pack_info;
  }

  dpm($output);

  $object = islandora_object_load('islandora:2');
  dpm($object->relationships->get('info:fedora/fedora-system:def/model#', 'hasModel'));

  dpm($object);
  dpm($object->models);
    return 'Testing';
}

