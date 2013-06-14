<?php

/**
 * Provide overrides for Drupal Commerce views.
 */

/**
 * Implements hook_views_default_views_alter().
 */
function commerce_overrides_views_default_views_alter(&$views) {

  // Overrides for "Shopping cart" cart view.
  if (isset($views['commerce_cart_form'])) {
    // Allow multiple values (order IDs) in argument.
    // @TODO: Shouldn't we loop on all displays here, just in case?
    $views['commerce_cart_form']->display['default']->display_options['arguments']['order_id']['break_phrase'] = TRUE;

    // Add store column next to product name.
    $commerce_store_field = array(
      'commerce_store' => array(
        'id' => 'commerce_store',
        'table' => 'field_data_commerce_store',
        'field' => 'commerce_store',
        'type' => 'commerce_store_reference_title_link',
      ),
    );
    // Find position where the field should be inserted
    // (before "commerce_customer_address" field).
    $field_keys = array_keys($views['commerce_cart_form']->display['default']->display_options['fields']);
    $insert_position = array_search('commerce_unit_price', $field_keys);
    // Slice the view in two and add new field inbetween.
    $views['commerce_cart_form']->display['default']->display_options['fields'] = array_merge(
      array_slice($views['commerce_cart_form']->display['default']->display_options['fields'], 0, $insert_position),
      $commerce_store_field,
      array_slice($views['commerce_cart_form']->display['default']->display_options['fields'], $insert_position)
    );
    // Add the field to Format Settings too.
    $views['commerce_cart_form']->display['default']->display_options['style_options']['columns']['commerce_store'] = 'commerce_store';
    $views['commerce_cart_form']->display['default']->display_options['style_options']['info']['commerce_store'] = array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    );
  }

  // Overrides for "Checkout" cart view.
  if (isset($views['commerce_cart_summary'])) {
    // Allow multiple values (order IDs) in argument.
    $views['commerce_cart_summary']->display['default']->display_options['arguments']['order_id']['break_phrase'] = TRUE;
  }

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
        'type' => 'commerce_store_reference_title_link',
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

  // Overrides for admin "Products" view.
  if (isset($views['commerce_products'])) {
    // Add store column next to product name.
    $commerce_store_field = array(
      'commerce_store' => array(
        'id' => 'commerce_store',
        'table' => 'field_data_commerce_store',
        'field' => 'commerce_store',
        'type' => 'commerce_store_reference_title_link',
      ),
    );
    // Find position where the field should be inserted
    // (before "type" field).
    $field_keys = array_keys($views['commerce_products']->display['default']->display_options['fields']);
    $insert_position = array_search('type', $field_keys);
    // Slice the view in two and add new field inbetween.
    $views['commerce_products']->display['default']->display_options['fields'] = array_merge(
      array_slice($views['commerce_products']->display['default']->display_options['fields'], 0, $insert_position),
      $commerce_store_field,
      array_slice($views['commerce_products']->display['default']->display_options['fields'], $insert_position)
    );
    // Add the field to Format Settings too.
    $views['commerce_products']->display['default']->display_options['style_options']['columns']['commerce_store'] = 'commerce_store';
    $views['commerce_products']->display['default']->display_options['style_options']['info']['commerce_store'] = array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    );
  }
}
