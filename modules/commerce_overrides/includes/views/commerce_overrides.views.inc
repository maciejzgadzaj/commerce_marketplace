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
    // @TODO: Doesn't this support weights or something? Ugly as hell this way...
    $fields = array();
    foreach ($views['commerce_cart_form']->display['default']->display_options['fields'] as $delta => $field) {
      $fields[$delta] = $field;
      if ($delta == 'line_item_title') {
        $fields['commerce_store'] = array(
          'id' => 'commerce_store',
          'table' => 'field_data_commerce_store',
          'field' => 'commerce_store',
          'type' => 'commerce_store_reference_title_link',
          'settings' => array(
            'field_formatter_extras_field_wrapper' => 'div',
            'field_formatter_extras_field_class' => '',
            'field_formatter_extras_value_class' => '',
            'field_formatter_extras_value_prefix' => '',
            'field_formatter_extras_value_suffix' => '',
          ),
        );
      }
    }
    $views['commerce_cart_form']->display['default']->display_options['fields'] = $fields;
  }

  // Overrides for "Checkout" cart view.
  if (isset($views['commerce_cart_summary'])) {
    // Allow multiple values (order IDs) in argument.
    $views['commerce_cart_summary']->display['default']->display_options['arguments']['order_id']['break_phrase'] = TRUE;
  }

}
