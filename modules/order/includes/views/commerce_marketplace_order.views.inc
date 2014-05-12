<?php

/**
 * @file
 * Overrides for Drupal Commerce views.
 */

/**
 * Implements hook_views_default_views_alter().
 *
 * @see commerce_order_ui_views_default_views()
 */
function commerce_marketplace_order_views_default_views_alter(&$views) {

  // Overrides for admin "Orders" view.
  if (isset($views['commerce_orders'])) {
    // Add "Order group" field to admin "Orders" view.
    $order_group_field = array(
      'order_group' => array(
        'id' => 'order_group',
        'table' => 'commerce_order',
        'field' => 'order_group',
        'label' => 'Order group',
      ),
    );
    // Find position where the field should be inserted (before "changed" field).
    $field_keys = array_keys($views['commerce_orders']->display['default']->display_options['fields']);
    $insert_position = array_search('changed', $field_keys);
    // Slice the view in two and add new field inbetween.
    $views['commerce_orders']->display['default']->display_options['fields'] = array_merge(
      array_slice($views['commerce_orders']->display['default']->display_options['fields'], 0, $insert_position),
      $order_group_field,
      array_slice($views['commerce_orders']->display['default']->display_options['fields'], $insert_position)
    );
    // Add the field to Format Settings too.
    $views['commerce_orders']->display['default']->display_options['style_options']['columns']['order_group'] = 'order_group';
    $views['commerce_orders']->display['default']->display_options['style_options']['info']['order_group'] = array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    );

    // Add "Store" field to admin "Orders" view.
    $commerce_store_field = array(
      'commerce_store' => array(
        'id' => 'commerce_store',
        'table' => 'field_data_commerce_store',
        'field' => 'commerce_store',
        'type' => 'entityreference_label',
      ),
    );
    // Find position where the field should be inserted
    // (before "commerce_customer_address" field).
    $field_keys = array_keys($views['commerce_orders']->display['default']->display_options['fields']);
    $insert_position = array_search('commerce_customer_address', $field_keys);
    // Slice the view in two and add new field inbetween.
    $views['commerce_orders']->display['default']->display_options['fields'] = array_merge(
      array_slice($views['commerce_orders']->display['default']->display_options['fields'], 0, $insert_position),
      $commerce_store_field,
      array_slice($views['commerce_orders']->display['default']->display_options['fields'], $insert_position)
    );
    // Add the field to Format Settings too.
    $views['commerce_orders']->display['default']->display_options['style_options']['columns']['commerce_store'] = 'commerce_store';
    $views['commerce_orders']->display['default']->display_options['style_options']['info']['commerce_store'] = array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    );
  }

  // Overrides for admin "Orders" view.
  if (isset($views['commerce_user_orders'])) {
    // Add relationship to the commerce_store entity.
    $views['commerce_user_orders']->display['default']->display_options['relationships']['commerce_store_target_id']['id'] = 'commerce_store_target_id';
    $views['commerce_user_orders']->display['default']->display_options['relationships']['commerce_store_target_id']['table'] = 'field_data_commerce_store';
    $views['commerce_user_orders']->display['default']->display_options['relationships']['commerce_store_target_id']['field'] = 'commerce_store_target_id';
    // Add "Store" field to user "Orders" view.
    $commerce_store_field = array(
      'title' => array(
        'id' => 'title',
        'table' => 'commerce_store',
        'field' => 'title',
        'relationship' => 'commerce_store_target_id',
        'label' => 'Store',
      ),
    );
    // Find position where the field should be inserted
    // (before "created" field).
    $field_keys = array_keys($views['commerce_user_orders']->display['default']->display_options['fields']);
    $insert_position = array_search('created', $field_keys);
    // Slice the view in two and add new field inbetween.
    $views['commerce_user_orders']->display['default']->display_options['fields'] = array_merge(
      array_slice($views['commerce_user_orders']->display['default']->display_options['fields'], 0, $insert_position),
      $commerce_store_field,
      array_slice($views['commerce_user_orders']->display['default']->display_options['fields'], $insert_position)
    );
    // Add the field to Format Settings too.
    $views['commerce_user_orders']->display['default']->display_options['style_options']['columns']['title'] = 'title';
    $views['commerce_user_orders']->display['default']->display_options['style_options']['info']['title'] = array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    );
  }

}
