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
