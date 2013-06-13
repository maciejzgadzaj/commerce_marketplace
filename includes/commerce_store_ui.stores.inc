<?php

/**
 * @file
 * Page callbacks and form builder functions for administering stores.
 */


/**
 * Menu callback: display a list of store types that the user can create.
 */
function commerce_store_ui_add_page() {
  $item = menu_get_item();
  $content = system_admin_menu_block($item);

  // Bypass the admin/commerce/stores/add listing if only one store type is
  // available.
  if (count($content) == 1) {
    $item = array_shift($content);
    drupal_goto($item['href']);
  }

  return theme('store_add_list', array('content' => $content));
}

/**
 * Displays the list of available store types for store creation.
 *
 * @ingroup themeable
 */
function theme_store_add_list($variables) {
  $content = $variables['content'];
  $output = '';

  if ($content) {
    $output = '<dl class="commerce-store-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l($item['title'], $item['href'], $item['localized_options']) . '</dt>';
      $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
    }
    $output .= '</dl>';
  }
  else {
    if (user_access('administer store types')) {
      $output = '<p>' . t('You have not created any store types yet. Go to the <a href="@create-store-type">store type creation page</a> to add a new store type.', array('@create-store-type' => url('admin/commerce/stores/types/add'))) . '</p>';
    }
    else {
      $output = '<p>' . t('No store types have been created yet for you to use.') . '</p>';
    }
  }

  return $output;
}

/**
 * Page callback: Displays store create/edit form.
 *
 * @param object $store
 *   The store object being edited by this form.
 *
 * @return array
 *   A render array for a page containing store create/edit form.
 *
 * @see commerce_store_ui_menu()
 * @see commerce_store_ui_store_form()
 */
function commerce_store_ui_store_form_wrapper($store) {
  // Include the forms file from the Store module.
  module_load_include('inc', 'commerce_store', 'includes/commerce_store_ui.forms');
  return drupal_get_form('commerce_store_ui_store_form', $store);
}

/**
 * Page callback: Displays confirmation form for deleting a store.
 *
 * @param $store
 *   The store object being deleted by this form.
 *
 * @return array
 *   A render array for a page containing confirmation form for deleting a store.
 *
 * @see commerce_store_ui_menu()
 * @see commerce_store_store_ui_delete_form()
 */
function commerce_store_ui_store_delete_form_wrapper($store) {
  // Include the forms file from the Store module.
  module_load_include('inc', 'commerce_store', 'includes/commerce_store_ui.forms');
  return drupal_get_form('commerce_store_ui_store_delete_form', $store);
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
function commerce_store_ui_store_payment_methods_overview($store) {
  $payment_methods = commerce_store_enabled_payment_methods();

  // Get payment methods which are enabled for this store.
  $store_enabled_methods = (!empty($store->data['payment_methods']['enabled'])) ? $store->data['payment_methods']['enabled'] : array();

  $output = '<p>' . t('Disabled payment methods can be enabled in the <em>Payment methods</em> section on the <a href="@url">store edit page</a>.', array(
      '%title' => $store->title,
      '@url' => url('admin/commerce/stores/' . $store->store_id, array(
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
    $method = !empty($store_enabled_methods[$method_id]) ? l($payment_method['title'], 'admin/commerce/stores/' . $store->store_id . '/payment_methods/' . $method_id) : $payment_method['title'];
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
function commerce_store_ui_store_payment_method_form_wrapper($store, $payment_method_id) {
  if (empty($store->data['payment_methods']['enabled'][$payment_method_id])) {
    return t('This payment method is not enabled for %title store. You can enable it in the <em>Payment methods</em> section on the <a href="@url">store edit page</a>.', array(
      '%title' => $store->title,
      '@url' => url('admin/commerce/stores/' . $store->store_id, array(
        'fragment' => 'edit-payment-methods',
        'query' => drupal_get_destination(),
      )),
    ));
  }
  // Include the forms file from the Store module.
  module_load_include('inc', 'commerce_store', 'includes/commerce_store_ui.forms');
  return drupal_get_form('commerce_store_ui_store_payment_method_form', $store, $payment_method_id);
}
