<?php

/**
 * @file
 * Overrides for Drupal Commerce views.
 */

/**
 * Implements hook_views_default_views_alter().
 */
function commerce_marketplace_product_views_default_views_alter(&$views) {

  // Overrides for admin "Products" view.
  if (isset($views['commerce_products'])) {
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
