<?php

/**
 * @file
 * Provides overrides for Commerce Checkout functions.
 */

/**
 * Checkout pane callback: returns the cart contents View for inclusion in the
 * checkout form.
 */
function commerce_overrides_cart_contents_pane_checkout_form($form, &$form_state, $checkout_pane, $order) {
  global $user;
  $pane_form = array();

  drupal_add_css(drupal_get_path('module', 'commerce_cart') . '/theme/commerce_cart.theme.css');

  // Extract the View and display keys from the cart contents pane setting.
  list($view_id, $display_id) = explode('|', variable_get('commerce_cart_contents_pane_view', 'commerce_cart_summary|default'));

  // Function commerce_overrides_cart_order_load_multiple() is provided by
  // commerce_cart override.
  $orders = commerce_overrides_cart_order_load_multiple($user->uid);
  $order_ids = implode(',', array_keys($orders));

  $pane_form['cart_contents_view'] = array(
    '#markup' => commerce_embed_view($view_id, $display_id, array($order_ids)),
  );

  return $pane_form;
}

/**
 * Submit handler used to redirect to the checkout page.
 *
 * Overrides commerce_checkout_line_item_views_form_submit().
 */
function commerce_overrides_checkout_line_item_views_form_submit($form, &$form_state) {
  global $user;

  // Set the order status to the first checkout page's status.
  $order_state = commerce_order_state_load('checkout');

  if ($orders = commerce_overrides_cart_order_load_multiple($user->uid)) {
    foreach ($orders as $order) {
      $order = commerce_order_status_update($order, $order_state['default_status'], TRUE);

      // Skip saving in the status update and manually save here to force a save
      // even when the status doesn't actually change.
      if (variable_get('commerce_order_auto_revision', TRUE)) {
        $order->revision = TRUE;
        $order->log = t('Customer proceeded to checkout using a submit button.');
      }

      commerce_order_save($order);
    }
  }
}

/**
 * Special submit handler for the continue button of the checkout form.
 *
 * Updates all orders statuses to reflect the checkout page.
 */
function commerce_overrides_checkout_form_submit($form, &$form_state) {
  global $user;
  $checkout_page = $form_state['checkout_page'];

  // If the form was submitted via the continue button...
  if (end($form_state['triggering_element']['#array_parents']) == 'continue') {
    // If there is another checkout page...
    if ($checkout_page['next_page']) {
      // Update the order status to reflect the next checkout page.
      if ($orders = commerce_overrides_cart_order_load_multiple($user->uid)) {
        foreach ($orders as $order) {
          commerce_order_status_update($order, 'checkout_' . $checkout_page['next_page'], FALSE, NULL, t('Customer continued to the next checkout page via a submit button.'));
        }
      }
    }
  }
}

/**
 * Special submit handler for the back button to avoid processing orders.
 *
 * Updates all orders statuses to reflect the checkout page.
 */
function commerce_overrides_checkout_form_back_submit($form, &$form_state) {
  global $user;
  // If there is a previous page...
  if ($previous_page = commerce_checkout_page_load($form_state['checkout_page']['prev_page'])) {
    // Update the order status for the checkout step.
    if ($orders = commerce_overrides_cart_order_load_multiple($user->uid)) {
      foreach ($orders as $order) {
        commerce_order_status_update($order, 'checkout_' . $previous_page['page_id'], FALSE, NULL, t('Customer returned to the previous checkout page via a submit button.'));
      }
    }
  }
}

/**
 * Special submit handler for the cancel button to avoid processing orders.
 *
 * Updates all orders statuses to the cart status.
 */
function commerce_overrides_checkout_form_cancel_submit($form, &$form_state) {
  global $user;

  if ($orders = commerce_overrides_cart_order_load_multiple($user->uid)) {
    foreach ($orders as $order) {

      // Update the order to the cart status.
      commerce_order_status_update($order, 'cart', TRUE);

      // Skip saving in the status update and manually save here to force a save
      // even when the status doesn't actually change.
      if (variable_get('commerce_order_auto_revision', TRUE)) {
        $order->revision = TRUE;
        $order->log = t('Customer manually canceled the checkout process.');
      }

      commerce_order_save($order);
    }
  }
}
