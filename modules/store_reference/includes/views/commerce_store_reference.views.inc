<?php

/**
 * Provide Store Reference related Views integration.
 */


/**
 * Implements hook_field_views_data().
 */
function commerce_store_reference_field_views_data($field) {
  $data = field_views_field_default_views_data($field);

  // Add a join to commerce_store so that it can be used by the inverse relationship.
  $data[_field_sql_storage_tablename($field)]['table']['join']['commerce_store'] = array(
    'left_field' => 'store_id',
    'field' => $field['field_name'] . '_store_id',
    'extra' => array(
      array('field' => 'deleted', 'value' => 0, 'numeric' => TRUE),
    ),
  );

  // Build an array of bundles the store reference field appears on.
  $bundles = array();

  foreach ($field['bundles'] as $entity => $entity_bundles) {
    $bundles[] = $entity . ' (' . implode(', ', $entity_bundles) . ')';
  }

  foreach ($data as $table_name => $table_data) {
    foreach ($table_data as $field_name => $field_data) {
      if (isset($field_data['filter']['field_name']) && $field_name != 'delta') {
        $data[$table_name][$field_name]['relationship'] = array(
          'title' => t('Referenced store'),
          'help' => t('Appears in: @bundles.', array('@bundles' => implode(', ', $bundles))),
          'base' => 'commerce_store',
          'base field' => 'store_id',
          'handler' => 'views_handler_relationship',
          'label' => t('Store'),
        );
      }
    }
  }

  return $data;
}

/**
 * Implements hook_views_data_alter().
 */
function commerce_store_reference_views_data_alter(&$data) {
  // Adds inverse relationships between the store and entity types referencing
  // it (for example, nodes).
  foreach (field_info_fields() as $field_name => $field) {
    if ($field['type'] !== 'commerce_store_reference') {
      continue;
    }

    foreach ($field['bundles'] as $entity_type => $bundles) {
      if ($entity_type == 'commerce_line_item') {
        continue;
      }

      $entity_info = entity_get_info($entity_type);
      $data['commerce_store'][$field['field_name']]['relationship'] = array(
        'handler' => 'views_handler_relationship',
        'base' => $entity_info['base table'],
        'base field' => $entity_info['entity keys']['id'],
        'relationship table' => _field_sql_storage_tablename($field),
        'relationship field' => 'entity_id',

        'label' => t('!entity_type referencing stores from !field_name', array('!entity_type' => $entity_info['label'], '!field_name' => $field['field_name'])),
        'title' => t('!entity_type referencing stores from !field_name', array('!entity_type' => $entity_info['label'],  '!field_name' => $field['field_name'])),
        'help' => t('Relate a store to the !entity_type referencing it.', array('!entity_type' => strtolower($entity_info['label']))),
      );
    }
  }
}
