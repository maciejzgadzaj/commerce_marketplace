<?php

/**
 * Form constructor for marketplace settings page.
 */
function commerce_marketplace_payment_settings_form($form, &$form_state) {

  $options = array(
    'main_store' => t('main store'),
    'merchants' => t('merchants'),
  );
  $form['commerce_store_payment_mode'] = array(
    '#type' => 'select',
    '#title' => t('Send payments to'),
    '#description' => t('Select how payments for marketplace orders (orders from multiple stores) should be handled. <em>Merchants</em> will try to send payments directly to relevant merchants (assuming they all have the same payment method supporting parallel payments enabled). <em>Main store</em> will send the payment to the marketplace owner, allowing for processing it manually later.'),
    '#options' => $options,
    '#default_value' => variable_get('commerce_store_payment_mode', COMMERCE_MARKETPLACE_PAYMENT_DEFAULT_MODE),
  );

  return system_settings_form($form);
}

/**
 * Page callback: Displays list of payment methods available for a store.
 *
 * @param $store
 *   The store object for which available payment methods are displayed.
 *
 * @return array
 *   Content for a page listing payment methods available for a store.
 *
 * @see commerce_store_ui_menu()
 */
function commerce_marketplace_payment_methods_overview($store) {
  $payment_methods = commerce_marketplace_payment_enabled_methods();

  // Get payment methods which are enabled for this store.
  $store_enabled_methods = (!empty($store->data['payment_methods']['enabled'])) ? $store->data['payment_methods']['enabled'] : array();

  $output = '<p>' . t('Disabled payment methods can be enabled in the <em>Payment methods</em> section on the <a href="@url">store edit page</a>.', array(
      '%title' => $store->title,
      '@url' => url('store/' . $store->store_id . '/edit', array(
          'fragment' => 'edit-payment-methods',
          'query' => drupal_get_destination(),
        )),
    )) . '</p>';
  $class = 'admin-list';
  if ($compact = system_admin_compact_mode()) {
    $class .= ' compact';
  }
  $output .= '<ul class="' . $class . '">';
  foreach ($payment_methods as $method_id => $payment_method) {
    // List only those payment methods which are enabled for this store.
    $method = !empty($store_enabled_methods[$method_id]) ? l($payment_method['title'], 'store/' . $store->store_id . '/payment-methods/' . $method_id) : $payment_method['title'];
    $output .= '<li class="leaf">' . $method;
    if (!$compact && isset($payment_method['description'])) {
      $output .= '<div class="description">' . filter_xss_admin($payment_method['description']) . '</div>';
    }
    $output .= '</li>';
  }
  $output .= '</ul>';

  return $output;
}

/**
 * Page callback: Displays store payment method edit form.
 *
 * @param object $store
 *   The store object the edited payment method belongs to.
 * @param string $payment_method_id
 *   The ID of the edited payment method.
 *
 * @return array
 *   Content for a page payment method settings form for a store.
 *
 * @see commerce_store_ui_menu()
 * @see theme_admin_block_content()
 */
function commerce_marketplace_payment_method_form_wrapper($store, $payment_method_id) {
  if (empty($store->data['payment_methods']['enabled'][$payment_method_id])) {
    return t('This payment method is not enabled for %title store. You can enable it in the <em>Payment methods</em> section on the <a href="@url">store edit page</a>.', array(
      '%title' => $store->title,
      '@url' => url('store/' . $store->store_id . '/edit', array(
          'fragment' => 'edit-payment-methods',
          'query' => drupal_get_destination(),
        )),
    ));
  }
  return drupal_get_form('commerce_marketplace_payment_method_form', $store, $payment_method_id);
}

/**
 * Form constructor for store payment methods settings form.
 *
 * @param object $store
 *   The store object to edit or for a create form an empty store object
 *   with only a store type defined.
 *
 * @see commerce_store_ui_store_payment_method_form_submit()
 *
 * @ingroup forms
 */
function commerce_marketplace_payment_method_form($form, &$form_state, $store, $payment_method_id) {
  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'commerce_marketplace_payment') . '/includes/commerce_marketplace_payment.admin.inc';

  $payment_methods = commerce_marketplace_payment_enabled_methods();
  $payment_method = $payment_methods[$payment_method_id];
  $settings_form_callback = $payment_method['callbacks']['settings_form'];

  if (function_exists($settings_form_callback)) {

    // Original payment method settings form.
    $defaults = !empty($store->data['payment_methods']['settings'][$payment_method_id]) ? $store->data['payment_methods']['settings'][$payment_method_id] : array();
    $form = call_user_func($settings_form_callback, $defaults);

    // Special processing for text_format form element, for which #default_value
    // is an array storing both value and format.
    // See issue https://drupal.org/node/2218609
    foreach (element_children($form) as $key) {
      if ($form[$key]['#type'] == 'text_format' && !empty($form[$key]['#default_value']) && is_array($form[$key]['#default_value'])) {
        $form[$key]['#format'] = $form[$key]['#default_value']['format'];
        $form[$key]['#default_value'] = $form[$key]['#default_value']['value'];
      }
    }

    // Keep submitted form elements in the tree so it's easier to populate
    // the form with defaults next time it is visited.
    $form['#tree'] = TRUE;

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save changes'),
      '#weight' => 50,
    );
    // Override any possible payment method settings form submit function.
    $form['#submit'] = array('commerce_marketplace_payment_method_form_submit');
  }
  else {
    $form['info'] = array(
      '#type' => 'markup',
      '#markup' => t('This payment method does not have any settings.'),
    );
  }

  return $form;
}

/**
 * Form submission handler for commerce_store_ui_store_payment_methods_form().
 *
 * @see commerce_store_ui_store_payment_methods_form_validate()
 */
function commerce_marketplace_payment_method_form_submit($form, &$form_state) {
  $store = $form_state['build_info']['args'][0];
  $payment_method_id = $form_state['build_info']['args'][1];

  $keys_to_remove = array_flip(array('form_build_id', 'form_token', 'form_id', 'submit', 'op'));
  $store->data['payment_methods']['settings'][$payment_method_id] = array_diff_key($form_state['values'], $keys_to_remove);

  // Save the payment method settings.
  commerce_store_save($store);
  drupal_set_message(t('Payment method settings saved.'));
}
