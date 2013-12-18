<?php

/**
 * Overrides an order total area handler that shows the order total field with
 * its components listed in the footer of a View to handle multiple orders.
 */
class commerce_marketplace_order_handler_area_order_total extends commerce_order_handler_area_order_total {

  function render($empty = FALSE) {
    if (!$empty || !empty($this->options['empty'])) {
      // First look for an order_id argument.
      foreach ($this->view->argument as $name => $argument) {
        if ($argument instanceof commerce_order_handler_argument_order_order_id) {
          // Load the order.
          if ($orders = commerce_order_load_multiple($argument->value)) {
            $order = reset($orders);

            // Prepare a display settings array.
            $display = array(
              'label' => 'hidden',
              'type' => 'commerce_price_formatted_components',
              'settings' => array(
                'calculation' => FALSE,
              ),
            );

            // If we have more than one order to display...
            if (count($argument->value) > 1) {
              // Calculate total of all orders.
              $total = commerce_marketplace_order_calculate_total($orders);
              // @TODO: This is ugly, we shouldn't do it, but for the moment,
              // being lazy and all, let's just use one of the orders and update
              // it with our super-total for display purposes.
              $order->commerce_order_total[LANGUAGE_NONE][0] = $total;
            }

            $field = field_view_field('commerce_order', $order, 'commerce_order_total', $display);
            return '<div class="commerce-order-handler-area-order-total">' . drupal_render($field) . '</div>';
          }
        }
      }
    }

    return '';
  }
}
