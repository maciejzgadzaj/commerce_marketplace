<?php

/**
 * Export Drupal Commerce line items to Views.
 */

/**
 * Implements hook_views_data_alter().
 *
 * Overrides handler for deleting a line item button (used on Shopping cart)
 * so that correct order object is used in views_form_submit() method when
 * removing a product from the shopping cart.
 *
 * @see commerce_line_item_handler_field_edit_delete::views_form_submit()
 * @see commerce_marketplace_line_item_handler_field_edit_delete::views_form_submit()
 */
function commerce_marketplace_line_item_views_data_alter(&$data) {
  $data['commerce_line_item']['edit_delete']['field']['handler'] = 'commerce_marketplace_line_item_handler_field_edit_delete';
}
