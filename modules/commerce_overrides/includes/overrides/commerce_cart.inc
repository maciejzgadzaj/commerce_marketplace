<?php

/**
 * @file
 * Provides overrides for Commerce Cart functions.
 */

/**
 * Overrides commerce_cart_add_to_cart_form_submit().
 *
 * Form submit handler: add the selected product to the cart.
 */
function commerce_overrides_cart_add_to_cart_form_submit($form, &$form_state) {
  $product_id = $form_state['values']['product_id'];
  $product = commerce_product_load($product_id);

  // If the line item passed to the function is new...
  if (empty($form_state['line_item']->line_item_id)) {
    // Create the new product line item of the same type.
    $line_item = commerce_product_line_item_new($product, $form_state['values']['quantity'], 0, $form_state['line_item']->data, $form_state['line_item']->type);

    // Allow modules to prepare this as necessary. This hook is defined by the
    // Product Pricing module.
    drupal_alter('commerce_product_calculate_sell_price_line_item', $line_item);

    // Remove line item field values the user didn't have access to modify.
    foreach ($form_state['values']['line_item_fields'] as $field_name => $value) {
      // Note that we're checking the Commerce Cart settings that we inserted
      // into this form element array back when we built the form. This means a
      // module wanting to alter a line item field widget to be available must
      // update both its form element's #access value and the field_access value
      // of the #commerce_cart_settings array.
      if (empty($form['line_item_fields'][$field_name]['#commerce_cart_settings']['field_access'])) {
        unset($form_state['values']['line_item_fields'][$field_name]);
      }
    }

    // Unset the line item field values array if it is now empty.
    if (empty($form_state['values']['line_item_fields'])) {
      unset($form_state['values']['line_item_fields']);
    }

    // Add field data to the line item.
    field_attach_submit('commerce_line_item', $line_item, $form['line_item_fields'], $form_state);

    // Process the unit price through Rules so it reflects the user's actual
    // purchase price.
    rules_invoke_event('commerce_product_calculate_sell_price', $line_item);

    // Only attempt an Add to Cart if the line item has a valid unit price.
    $line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $line_item);
    if (!is_null($line_item_wrapper->commerce_unit_price->value())) {
      // Add the product to the specified shopping cart.
      $form_state['line_item'] = commerce_overrides_cart_product_add(
        $form_state['values']['uid'],
        $line_item,
        isset($line_item->data['context']['add_to_cart_combine']) ? $line_item->data['context']['add_to_cart_combine'] : TRUE
      );
    }
    else {
      drupal_set_message(t('%title could not be added to your cart.', array('%title' => $product->title)), 'error');
    }
  }
}

/**
 * Overrides commerce_cart_product_add().
 *
 * Adds the specified product to a customer's shopping cart.
 */
function commerce_overrides_cart_product_add($uid, $line_item, $combine = TRUE) {
  // Do not add the line item if it doesn't have a unit price.
  $line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $line_item);

  if (is_null($line_item_wrapper->commerce_unit_price->value())) {
    return FALSE;
  }

  // First attempt to load the customer's shopping cart order.
  $store = $line_item_wrapper->commerce_product->commerce_store->value();
  $order = commerce_overrides_cart_order_load($uid, $store->store_id);

  // If no order existed, create one now.
  if (empty($order)) {
    // We need to get order group ID before calling commerce_cart_order_new(),
    // as it will already save a new order with order group ID set to 0.
    $order_group = commerce_overrides_get_current_order_group_id();
    $order = commerce_cart_order_new($uid);
    $order->order_group = $order_group;
  }

  // Set the incoming line item's order_id.
  $line_item->order_id = $order->order_id;

  // Wrap the order for easy access to field data.
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);

  // Add store ID to order details.
  $order_wrapper->commerce_store = $store->store_id;

  // Extract the product and quantity we're adding from the incoming line item.
  $product = $line_item_wrapper->commerce_product->value();
  $quantity = $line_item->quantity;

  // Invoke the product prepare event with the shopping cart order.
  rules_invoke_all('commerce_cart_product_prepare', $order, $product, $line_item->quantity);

  // Determine if the product already exists on the order and increment its
  // quantity instead of adding a new line if it does.
  $matching_line_item = NULL;

  // If we are supposed to look for a line item to combine into...
  if ($combine) {
    // Generate an array of properties and fields to compare.
    $comparison_properties = array('type', 'commerce_product');

    // Add any field that was exposed on the Add to Cart form to the array.
    // TODO: Bypass combination when an exposed field is no longer available but
    // the same base product is added to the cart.
    foreach (field_info_instances('commerce_line_item', $line_item->type) as $info) {
      if (!empty($info['commerce_cart_settings']['field_access'])) {
        $comparison_properties[] = $info['field_name'];
      }
    }

    // Allow other modules to specify what properties should be compared when
    // determining whether or not to combine line items.
    drupal_alter('commerce_cart_product_comparison_properties', $comparison_properties);

    // Loop over each line item on the order.
    foreach ($order_wrapper->commerce_line_items as $delta => $matching_line_item_wrapper) {
      // Examine each of the comparison properties on the line item.
      foreach ($comparison_properties as $property) {
        // If any property does not match the same property on the incoming line
        // item...
        if ($matching_line_item_wrapper->{$property}->raw() != $line_item_wrapper->{$property}->raw()) {
          // Continue the loop with the next line item.
          continue 2;
        }
      }

      // If every comparison line item matched, combine into this line item.
      $matching_line_item = $matching_line_item_wrapper->value();
      break;
    }
  }

  // If no matching line item was found...
  if (empty($matching_line_item)) {
    // Save the incoming line item now so we get its ID.
    commerce_line_item_save($line_item);

    // Add it to the order's line item reference value.
    $order_wrapper->commerce_line_items[] = $line_item;
  }
  else {
    // Increment the quantity of the matching line item, update the data array,
    // and save it.
    $matching_line_item->quantity += $quantity;
    $matching_line_item->data = array_merge($line_item->data, $matching_line_item->data);

    commerce_line_item_save($matching_line_item);

    // Clear the line item cache so the updated quantity will be available to
    // the ensuing load instead of the original quantity as loaded above.
    entity_get_controller('commerce_line_item')->resetCache(array($matching_line_item->line_item_id));

    // Update the line item variable for use in the invocation and return value.
    $line_item = $matching_line_item;
  }

  // Save the updated order.
  commerce_order_save($order);

  // Invoke the product add event with the newly saved or updated line item.
  rules_invoke_all('commerce_cart_product_add', $order, $product, $quantity, $line_item);

  // Return the line item.
  return $line_item;
}

/**
 * Overrides commerce_cart_checkout_form_cancel_submit().
 *
 * Submit handler to take back the order to cart status on cancel in checkout.
 */
function commerce_overrides_cart_checkout_form_cancel_submit($form, &$form_state) {
  global $user;

  if ($orders = commerce_overrides_cart_order_load_multiple($user->uid)) {
    foreach ($orders as $order) {
      // Update the order to the cart status.
      $order = commerce_order_status_update($order, 'cart', TRUE);

      // Skip saving in the status update and manually save here to force a save
      // even when the status doesn't actually change.
      if (variable_get('commerce_order_auto_revision', TRUE)) {
        $form_state['order']->revision = TRUE;
        $form_state['order']->log = t('Customer manually canceled the checkout process.');
      }

      commerce_order_save($order);
    }
  }

  drupal_set_message(t('Checkout of your current order has been canceled and may be resumed when you are ready.'));

  // Redirect to cart on cancel.
  $form_state['redirect'] = 'cart';
}

/**
 * Overrides commerce_cart_line_item_views_form_submit().
 *
 * Submit handler to show the shopping cart updated message.
 */
function commerce_overrides_cart_line_item_views_form_submit($form, &$form_state) {
  global $user;

  if ($orders = commerce_overrides_cart_order_load_multiple($user->uid)) {
    foreach ($orders as $order) {
      // Reset the status of the order to cart.
      $order = commerce_order_status_update($order, 'cart', TRUE);

      // Skip saving in the status update and manually save here to force a save
      // even when the status doesn't actually change.
      if (variable_get('commerce_order_auto_revision', TRUE)) {
        $order->revision = TRUE;
        $order->log = t('Customer updated the order via the shopping cart form.');
      }

      commerce_order_save($order);
    }
  }

  drupal_set_message(t('Your shopping cart has been updated.'));
}

/**
 * Extends commerce_cart_order_load().
 *
 * Loads all shopping cart orders for the specified user and specified store.
 */
function commerce_overrides_cart_order_load_multiple($uid = 0, $store_id = NULL) {
  // Retrieve the order ID for the specified user's current shopping cart.
  $order_ids = commerce_overrides_cart_order_ids($uid, $store_id);

  // If a valid cart order ID exists for the user, return it now.
  return !empty($order_ids) ? commerce_order_load_multiple($order_ids) : array();
}

/**
 * Extends commerce_cart_order_load().
 *
 * Loads the most recent shopping cart order for the specified user and
 * specified store.
 */
function commerce_overrides_cart_order_load($uid = 0, $store_id = NULL) {
  $orders = commerce_overrides_cart_order_load_multiple($uid, $store_id);
  return reset($orders);
}

/**
 * Extends commerce_cart_order_id().
 *
 * Returns the current cart order ID for the given user.
 */
function commerce_overrides_cart_order_ids($uid = 0, $store_id = NULL) {
  // Cart order IDs will be cached keyed by $uid.
  $cart_order_ids = &drupal_static(__FUNCTION__);

  // Cache the user's cart order ID if it hasn't been set already.
  if (isset($cart_order_ids[$uid][$store_id])) {
    return $cart_order_ids[$uid][$store_id];
  }

  // First let other modules attempt to provide a valid order ID for the given
  // uid. Instead of invoking hook_commerce_cart_order_id() directly, we invoke
  // it in each module implementing the hook and return the first valid order ID
  // returned (if any).
  foreach (module_implements('commerce_cart_order_id') as $module) {
    $order_ids = module_invoke($module, 'commerce_overrides_cart_order_ids', $uid, $store_id);

    // If a hook said the user should not have a cart, that overrides any other
    // potentially valid order ID. Return FALSE now.
    if ($order_ids === FALSE) {
      return $cart_order_ids[$uid][$store_id] = array();
    }

    // If a hook returned a single order ID instead of an array, convert it.
    if (!is_array($order_ids)) {
      $order_ids = array($order_ids);
    }

    // Otherwise only return a valid order IDs.
    if (!empty($order_ids) && is_array($order_ids)) {
      return $cart_order_ids[$uid][$store_id] = $order_ids;
    }
  }

  // If no order ID exists in anonymous user's session.
  if (!$uid && !commerce_cart_order_session_exists()) {
    return $cart_order_ids[$uid][$store_id] = array();
  }

  // Create an array of valid shopping cart order statuses.
  $status_ids = array_keys(commerce_order_statuses(array('cart' => TRUE)));

  // Look for the user's shopping cart orders for specific $store_id.
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'commerce_order', '=')
    ->propertyCondition('uid', $uid)
    ->propertyCondition('status', $status_ids, 'IN')
    ->propertyOrderBy('order_id', 'DESC');
  // If store ID was provided look for order in specific store only.
  if ($store_id) {
    $query->fieldCondition('commerce_store', 'store_id', $store_id, '=');
  }
  // For anonymous users look for a shopping cart order ID in the session.
  if (!$uid) {
    $query->propertyCondition('order_id', commerce_cart_order_session_order_ids(), 'IN');
  }
  $entities = $query->execute();

  return $cart_order_ids[$uid][$store_id] = !empty($entities['commerce_order']) ? array_keys($entities['commerce_order']) : FALSE;
}

/**
 * Overrides commerce_cart_checkout_router().
 *
 * Redirects invalid checkout attempts or displays the checkout form if valid.
 */
function commerce_overrides_cart_checkout_router() {
  global $user;

  // Load the shopping cart orders.
  $orders = commerce_overrides_cart_order_load_multiple($user->uid);

  // If no shopping cart order could be found, redirect away from checkout.
  if (empty($orders) || commerce_overrides_order_quantity($orders) == 0) {
    drupal_set_message(t('Add some items to your cart and then try checking out.'));
    drupal_goto(variable_get('commerce_checkout_empty_redirect', ''));
  }

  $order_group = commerce_overrides_get_current_order_group_id();
  drupal_goto('checkout/' . $order_group);
}

/**
 * Overrides commerce_cart_view().
 *
 * Displays the shopping cart form and associated information, providing option
 * to display a view of line items from multiple user's orders (for multiple
 * stores).
 */
function commerce_overrides_cart_view() {
  global $user;

  // Default to displaying an empty message.
  $content = theme('commerce_cart_empty_page');

  // First check to make sure we have a valid order.
  if ($orders = commerce_overrides_cart_order_load_multiple($user->uid)) {

    // Calculate total quantity in all orders for all stores.
    $quantity = 0;
    foreach ($orders as $order) {
      $wrapper = entity_metadata_wrapper('commerce_order', $order);
      $quantity += commerce_line_items_quantity($wrapper->commerce_line_items, commerce_product_line_item_types());
    }

    // Only show the cart form if we found product line items.
    if ($quantity > 0) {

      // Order ID view contextual filter should be a string
      // listing all order IDs separated by a comma.
      $order_ids = implode(',', array_keys($orders));

      // Add the form for editing the cart contents.
      $content = commerce_embed_view('commerce_cart_form', 'default', array($order_ids), 'cart');
    }
  }

  return $content;
}
