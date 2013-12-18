<?php

/**
 * @file
 * Overrides for Drupal Commerce views.
 */

/**
 * Implements hook_views_default_views_alter().
 */
function commerce_marketplace_cart_views_default_views_alter(&$views) {

  // Overrides for "Shopping cart" block view.
  if (isset($views['commerce_cart_block'])) {
    // Allow multiple values (order IDs) in argument.
    $views['commerce_cart_block']->display['default']->display_options['arguments']['order_id']['break_phrase'] = TRUE;
  }

  // Overrides for "Shopping cart" cart view.
  if (isset($views['commerce_cart_form'])) {
    // Allow multiple values (order IDs) in argument.
    $views['commerce_cart_form']->display['default']->display_options['arguments']['order_id']['break_phrase'] = TRUE;

    // Add store column next to product name.
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

}
