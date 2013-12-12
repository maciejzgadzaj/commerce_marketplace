<?php

/**
 * @file
 * Provides overrides for Commerce Checkout functions.
 */

/**
 * Overrides commerce_checkout_router().
 *
 * Redirects invalid checkout attempts or displays the checkout form if valid.
 */
function commerce_overrides_checkout_router($order_group, $checkout_page = NULL) {
  $checkout_pages = commerce_checkout_pages();

  // If no checkout page is specified, default to the first one.
  if (empty($checkout_page)) {
    $checkout_page = reset($checkout_pages);
  }

  $orders = commerce_overrides_order_group_load($order_group);

  // Few access checks to be done for each order.
  foreach ($orders as $order) {
    // If the user does not have access to checkout the order, return a 404. We
    // could return a 403, but then the user would know they've identified a
    // potentially valid checkout URL.
    if (!commerce_checkout_access($order)) {
      return MENU_NOT_FOUND;
    }

    // If the user is attempting to access an inaccessible page for their order,
    // redirect them to the proper page.
    if (!commerce_checkout_page_access($checkout_page, $order)) {
      $target_uri = commerce_checkout_order_uri($order);

      // Only redirect if the target page is different from the page the user was
      // trying to access. Otherwise give a 403 error.
      if (!empty($target_uri) && $target_uri !== $_GET['q']) {
        drupal_goto($target_uri);
      }
      else {
        return MENU_ACCESS_DENIED;
      }
    }
  }

  // If there are no line items on the order, redirect away.
  if (commerce_overrides_order_quantity($orders) == 0) {
    drupal_goto('<front>');
  }

  // Now, this where the shite starts. We're managing multiple orders here,
  // but most of the code is written to work with only one order. For its sake
  // then we'll just get the last order from the whole group and pass it there,
  // hoping it won't break anything, and overriding most important functions.
  $order = end($orders);

  // Prior to displaying the checkout form, allow other modules to route the
  // checkout form.
  module_invoke_all('commerce_checkout_router', $order, $checkout_page);

  // Update the page title if specified.
  if (!empty($checkout_page['title'])) {
    drupal_set_title($checkout_page['title']);
  }

  module_load_include('inc', 'commerce_checkout', 'includes/commerce_checkout.pages');
  return drupal_get_form('commerce_checkout_form_' . $checkout_page['page_id'], $order, $checkout_page, $order_group, $orders);
}

/**
 * Checkout pane callback: returns the cart contents view for inclusion in the
 * checkout form on the "Checkout" page.
 *
 * Overrides commerce_cart_contents_pane_checkout_form().
 *
 * @see commerce_cart_contents_pane_checkout_form()
 * @see commerce_overrides_commerce_checkout_pane_info_alter()
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
 * Checkout pane callback: returns the cart contents view for inclusion in the
 * checkout form on the "Review" checkout page.
 *
 * Overrides commerce_cart_contents_pane_review().
 *
 * @see commerce_cart_contents_pane_review()
 * @see commerce_overrides_commerce_checkout_pane_info_alter()
 */
function commerce_overrides_cart_contents_pane_review($form, $form_state, $checkout_pane, $order) {
  global $user;
  drupal_add_css(drupal_get_path('module', 'commerce_cart') . '/theme/commerce_cart.theme.css');

  // Extract the View and display keys from the cart contents pane setting.
  list($view_id, $display_id) = explode('|', variable_get('commerce_cart_contents_pane_view', 'commerce_cart_summary|default'));

  // Function commerce_overrides_cart_order_load_multiple() is provided by
  // commerce_cart override.
  $orders = commerce_overrides_cart_order_load_multiple($user->uid);
  $order_ids = implode(',', array_keys($orders));

  return commerce_embed_view($view_id, $display_id, array($order_ids));
}

/**
 * Overrides commerce_checkout_line_item_views_form_submit().
 *
 * Submit handler used to redirect to the checkout page.
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

  // Redirect to the checkout page if specified.
  if ($form_state['triggering_element']['#value'] == $form['actions']['checkout']['#value']) {
    $order_group = commerce_overrides_get_current_order_group_id();
    $form_state['redirect'] = 'checkout/' . $order_group;
  }
}

/**
 * Overrides commerce_checkout_form_submit().
 *
 * Special submit handler for the continue button of the checkout form.
 * Updates all orders statuses to reflect the checkout page.
 */
function commerce_overrides_checkout_form_submit($form, &$form_state) {
  $checkout_page = $form_state['checkout_page'];
  $order_group = $form_state['build_info']['args'][2];
  $orders = $form_state['build_info']['args'][3];

  // If we are going to redirect with checkout pane messages stored in the form
  // state, they will not be displayed on a subsequent form build like normal.
  // Move them out of the form state messages array and into the current
  // session's general message array instead.
  if (!empty($form_state['storage']['messages'])) {
    foreach ($form_state['storage']['messages'] as $pane_id => $pane_messages) {
      $_SESSION['messages'] = array_merge_recursive($_SESSION['messages'], $pane_messages);
    }
  }

  // If the form was submitted via the continue button...
  if (end($form_state['triggering_element']['#array_parents']) == 'continue') {
    // If there is another checkout page...
    if ($checkout_page['next_page']) {
      // Update all order statuses to reflect the next checkout page.
      foreach ($orders as $order) {
        $order = commerce_order_status_update($order, 'checkout_' . $checkout_page['next_page'], FALSE, NULL, t('Customer continued to the next checkout page via a submit button.'));

        // If it happens to be the complete page, process completion now.
        if ($checkout_page['next_page'] == 'complete') {
          commerce_checkout_complete($order);
        }
      }

      // Redirect to the next checkout page.
      $form_state['redirect'] = 'checkout/' . $order_group . '/' . $checkout_page['next_page'];
    }
  }
}

/**
 * Overrides commerce_checkout_form_back_submit().
 *
 * Special submit handler for the back button to avoid processing orders.
 * Updates all orders statuses to reflect the checkout page.
 */
function commerce_overrides_checkout_form_back_submit($form, &$form_state) {
  $order_group = $form_state['build_info']['args'][2];
  $orders = $form_state['build_info']['args'][3];

  // If there is a previous page...
  if ($previous_page = commerce_checkout_page_load($form_state['checkout_page']['prev_page'])) {

    // Move the form back to that page.
    if ($previous_page['prev_page']) {
      $form_state['redirect'] = 'checkout/' . $order_group . '/' . $previous_page['page_id'];
    }
    else {
      $form_state['redirect'] = 'checkout/' . $order_group;
    }

    // Update all order statuses for the checkout step.
    foreach ($orders as $order) {
      commerce_order_status_update($order, 'checkout_' . $previous_page['page_id'], FALSE, NULL, t('Customer returned to the previous checkout page via a submit button.'));
    }
  }
}
