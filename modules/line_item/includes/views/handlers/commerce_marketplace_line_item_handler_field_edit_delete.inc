<?php

/**
 * @file
 * Field handler to present a button to remove a line item. It's a dummy
 * handler, most part of the implementation is done via pre and post render
 * hooks.
 */

/**
 * Field handler to present a button to delete a line item.
 *
 * @see commerce_line_item_handler_field_edit_delete::views_form_submit()
 */
class commerce_marketplace_line_item_handler_field_edit_delete extends commerce_line_item_handler_field_edit_delete {

  function views_form_submit($form, &$form_state) {
    $field_name = $this->options['id'];

    foreach (element_children($form[$field_name]) as $row_id) {
      // Check for the removal of an item.
      if ($form_state['triggering_element']['#name'] == 'delete-line-item-' . $row_id) {
        $line_item_id = $form[$field_name][$row_id]['#line_item_id'];
        // What has changed here compared to standard DC is that we are loading
        // order based on order_id provided in line_item object.
        $line_item = commerce_line_item_load($line_item_id);
        $order = commerce_order_load($line_item->order_id);
        // TODO: Remove this dependence on the Cart module API.
        commerce_cart_order_product_line_item_delete($order, $line_item_id);
      }
    }
  }

}
