<?php

/**
 * @file
 * Provides overrides for Commerce Order functions.
 */

/**
 * Overrides commerce_order_calculate_total().
 *
 * Calculates the order total, updating the commerce_order_total field data in
 * the order object this function receives.
 */
function commerce_overrides_order_calculate_total($orders) {
  // First determine the currency to use for the order total.
  $currency_code = commerce_default_currency();
  $currencies = array();

  foreach ($orders as $key => $order) {
    $order_wrappers[$key] = entity_metadata_wrapper('commerce_order', $order);

    // Populate an array of how many line items on the order use each currency.
    foreach ($order_wrappers[$key]->commerce_line_items as $delta => $line_item_wrapper) {
      $line_item_currency_code = $line_item_wrapper->commerce_total->currency_code->value();

      if (!in_array($line_item_currency_code, array_keys($currencies))) {
        $currencies[$line_item_currency_code] = 1;
      }
      else {
        $currencies[$line_item_currency_code]++;
      }
    }

  }
  reset($currencies);

  // If only one currency is present on the order, use that to calculate the
  // order total.
  if (count($currencies) == 1) {
    $currency_code = key($currencies);
  }
  elseif (in_array(commerce_default_currency(), array_keys($currencies))) {
    // Otherwise use the site default currency if it's in the order.
    $currency_code = commerce_default_currency();
  }
  elseif (count($currencies) > 1) {
    // Otherwise use the first currency on the order. We do this instead of
    // trying to determine the most dominant currency for now because using the
    // first currency leaves the option open for a UI based module to let
    // customers reorder the items in the cart by currency to get the order
    // total in a different currency. The currencies array still contains useful
    // data, though, should we decide to expand on the count by currency approach.
    $currency_code = key($currencies);
  }

  // Initialize the order total with the selected currency.
  $total = array(
    'amount' => 0,
    'currency_code' => $currency_code,
    'data' => array(),
  );

  // Then loop over each line item in each order
  // and add its total to the order total.
  foreach ($order_wrappers as $key => $order_wrapper) {
    foreach ($order_wrapper->commerce_line_items as $delta => $line_item_wrapper) {
      // Convert the line item's total to the order's currency for totalling.
      $component_total = commerce_price_component_total($line_item_wrapper->commerce_total->value());

      // Add the totals.
      $total['amount'] += commerce_currency_convert(
        $component_total['amount'],
        $component_total['currency_code'],
        $currency_code
      );

      // Combine the line item total's component prices into the order total.
      $total['data'] = commerce_price_components_combine($total, $line_item_wrapper->commerce_total->value());
    }
  }

  return $total;
}

/**
 * Returns the total quantity of an array of line items in all orders.
 */
function commerce_overrides_order_quantity($orders) {
  $quantity = 0;
  foreach ($orders as $key => $order) {
    $wrapper = entity_metadata_wrapper('commerce_order', $order);
    $quantity += commerce_line_items_quantity($wrapper->commerce_line_items, commerce_product_line_item_types());
  }
  return $quantity;
}

/**
 * Returns all orders from one order group.
 */
function commerce_overrides_order_group_load($order_group) {
  return !empty($order_group) ? commerce_order_load_multiple(NULL, array('order_group' => array($order_group))) : array();
}

/**
 * Returns order group ID for the current user.
 */
function commerce_overrides_get_current_order_group_id() {
  global $user;
  $orders = commerce_overrides_cart_order_load_multiple($user->uid);
  if (!empty($orders)) {
    $order = reset($orders);
    return $order->order_group;
  }
  else {
    $last_group = db_select('commerce_order','co')->fields('co', array('order_group'))->orderby('order_group','DESC')->range(0,1)->execute()->fetchField();
    // If no order exists yet, just return 1 as the very first order group ID.
    return $last_group ? ++$last_group : 1;
  }
}
