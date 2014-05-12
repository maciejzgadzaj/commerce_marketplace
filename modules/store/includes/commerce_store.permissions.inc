<?php

/**
 * Implements hook_permission().
 */
function commerce_store_permission() {
  $permissions = array(
    'administer store types' => array(
      'title' => t('Administer store types'),
      'description' => t('Allows users to configure store types and their fields.'),
      'restrict access' => TRUE,
    ),
  );

  $permissions += commerce_entity_access_permissions('commerce_store');

  // Viewing stores is ok for anyone.
  unset($permissions['view any commerce_store entity']['restrict access']);
  // The same applies to all specific commerce_store bundles.
  $entity_info = entity_get_info('commerce_store');
  if (!empty($entity_info['entity keys']['bundle'])) {
    foreach ($entity_info['bundles'] as $bundle_name => $bundle_info) {
      unset($permissions['view any commerce_store entity of bundle ' . $bundle_name]['restrict access']);
    }
  }

  // Extra permissions for products and orders in own store(s).
  foreach (array('commerce_product', 'commerce_order') as $entity_type) {
    if (module_exists($entity_type)) {
      $permissions += commerce_store_entity_access_permissions($entity_type);
    }
  }

  return $permissions;
}

/**
 * Return permission names for a given entity type.
 *
 * @see commerce_entity_access_permissions()
 */
function commerce_store_entity_access_permissions($entity_type) {
  $entity_info = entity_get_info($entity_type);
  $labels = $entity_info['permission labels'];

  $permissions = array();

  // Generic create and edit permissions.
  $permissions['view any ' . $entity_type . ' entity in own store'] = array(
    'title' => t('View any @entity_type of any type in own store', array('@entity_type' => $labels['singular'])),
  );
  $permissions['edit any ' . $entity_type . ' entity in own store'] = array(
    'title' => t('Edit any @entity_type of any type in own store', array('@entity_type' => $labels['singular'])),
  );

  // Per-bundle create and edit permissions.
  if (!empty($entity_info['entity keys']['bundle'])) {
    foreach ($entity_info['bundles'] as $bundle_name => $bundle_info) {
      $permissions['view any ' . $entity_type . ' entity of bundle ' . $bundle_name . ' in own store'] = array(
        'title' => t('View any %bundle @entity_type in own store', array('@entity_type' => $labels['singular'], '%bundle' => $bundle_info['label'])),
      );
      $permissions['edit any ' . $entity_type . ' entity of bundle ' . $bundle_name . ' in own store'] = array(
        'title' => t('Edit any %bundle @entity_type in own store', array('@entity_type' => $labels['singular'], '%bundle' => $bundle_info['label'])),
      );
    }
  }

  return $permissions;
}

/**
 * Another generic implementation (besides commerce_entity_access_query_alter())
 * of hook_query_alter() for selected Drupal Commerce entities. This is just to
 * make sure that for all commerce_order and commerce_product select queries
 * there always is a join with commerce_store table added, so we could call our
 * hook_commerce_entity_access_condition_ENTITY_TYPE_alter() implementations
 * below.
 *
 * @see commerce_entity_access_query_alter()
 */
function commerce_store_entity_access_query_alter(QueryAlterableInterface $query, $entity_type, $base_table = NULL) {
  $tables = &$query->getTables();

  // If join with commerce_store table was not already added by a view...
  if (!in_array('commerce_store_field_data_commerce_store', array_keys($tables))) {
    // Read the base table from the query if available or default to the first
    // table in the query's tables array.
    if (!isset($base_table) && !$base_table = $query->getMetaData('base_table')) {
      // Assume that the base table is the first table if not set. It will result
      // in an invalid query if the first table is not the table we expect,
      // forcing the caller to actually properly pass a base table in that case.
      reset($tables);
      $base_table = key($tables);
    }
    $base_table_schema = drupal_get_schema($base_table);
    $base_table_primary_key = reset($base_table_schema['primary key']);
    // Normally table alias should be the same as table name, there are
    // cases though when the alias is different, for example in
    // commerce_payment_query_commerce_payment_transaction_access_alter(), where
    // table name is "commerce_order", but alias is "co". In such cases we need
    // to loop through query tables to find the correct one.
    $base_table_alias = $base_table;
    if (!in_array($base_table_alias, array_keys($tables))) {
      foreach ($tables as $table) {
        if ($table['table'] == $base_table) {
          $base_table_alias = $table['alias'];
        }
      }
    }

    // Add join with intermediary field_data_commerce_store table.
    if (!in_array('field_data_commerce_store', array_keys($tables))) {
      $query->leftJoin(
        'field_data_commerce_store',
        'field_data_commerce_store',
        "$base_table_alias.$base_table_primary_key = field_data_commerce_store.entity_id AND field_data_commerce_store.entity_type = :entity_type AND field_data_commerce_store.deleted = :deleted",
        array(':entity_type' => $base_table, ':deleted' => 0)
      );
    }
    // Add join with main commerce_store table.
    $query->leftJoin(
      'commerce_store',
      'commerce_store_field_data_commerce_store',
      "field_data_commerce_store.commerce_store_target_id = commerce_store_field_data_commerce_store.store_id",
      array()
    );
  }
}

/**
 * Implements hook_commerce_entity_access_condition_ENTITY_TYPE_alter().
 *
 * This alter hook allows the Store module to add conditions to the query used
 * to determine if a user has view access to a given product. The Store module
 * will always grant users access to view products in stores they own
 * (independent of any permission settings).
 */
function commerce_store_commerce_entity_access_condition_commerce_product_alter(&$conditions, $context) {
  // Make sure that a merchant (a user which owns at least one store) has
  // access to all products belonging to their store. This is used for
  // "Merchant products" view in user profile (/user/%/merchant/products).
  if (user_access('view any ' . $context['entity_type'] . ' entity in own store', $context['account'])) {
    // Actually this condition already is in the query, but we need to add
    // something to avoid commerce_entity_access_query_alter() adding default
    // "where('1 = 0')" condition if no other conditions are defined.
    $conditions->condition('commerce_store_field_data_commerce_store.uid', $context['account']->uid, '=');
  }
  // TODO: add access check for specific product types.
}

/**
 * Implements hook_commerce_entity_access_condition_ENTITY_TYPE_alter().
 *
 * This alter hook allows the Store module to add conditions to the query used
 * to determine if a user has view access to a given order or product, which has
 * a relationship to a store added.
 */
function commerce_store_commerce_entity_access_condition_commerce_store_alter(&$conditions, $context) {
  // Although this hook is invoked with entity_type in $context set to
  // commerce_store, what we really are interested in here is either product
  // order entity, which should be available in context's base_table key.
  // TODO: Should we worry that base table name could be different from entity
  // type name?
  if (user_access('view any ' . $context['base_table'] . ' entity in own store', $context['account'])) {
    $conditions->condition('commerce_store_field_data_commerce_store.uid', $context['account']->uid, '=');
  }
  // TODO: add access check for specific product and order types.
}

/**
 * Implementation of hook_query_commerce_store_access_alter().
 */
function commerce_store_query_commerce_store_access_alter(QueryAlterableInterface $query) {
  return commerce_entity_access_query_alter($query, 'commerce_store');
}

/**
 * Extends commerce_order_admin_order_view_access().
 *
 * Menu item access callback: prevent view access to the admin order display
 * for customers who have 'view' access to the order but not administration
 * pages nor orders in their own stores.
 */
function commerce_store_admin_order_view_access($order) {
  $access = commerce_order_admin_order_view_access($order);
  return $access || user_access('view any commerce_order entity in own store') && commerce_order_access('view', $order);
}

/**
 * Access callback: determines access to a store's translation tab.
 */
function commerce_store_entity_translation_tab_access($store) {
  return entity_translation_tab_access('commerce_store', $store);
}
