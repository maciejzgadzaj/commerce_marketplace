<?php

/**
 * @file
 */

/**
 * Menu callback: display an overview of available types.
 */
function commerce_store_ui_types_overview() {
  drupal_add_css(drupal_get_path('module', 'commerce_store') . '/theme/commerce_store.admin.css');

  $header = array(
    t('Name'),
    t('Operations'),
  );

  $rows = array();

  // Loop through all defined store types.
  foreach (commerce_store_types() as $type => $store_type) {
    // Build the operation links for the current store type.
    $links = menu_contextual_links('commerce-store-type', 'admin/commerce/stores/types', array(commerce_store_type_to_arg($type)));

    // Add the store type's row to the table's rows array.
    $rows[] = array(
      theme('store_type_admin_overview', array('store_type' => $store_type)),
      theme('links', array('links' => $links, 'attributes' => array('class' => 'links inline operations'))),
    );
  }

  // If no store types are defined...
  if (empty($rows)) {
    // Add a standard empty row with a link to add a new store type.
    $rows[] = array(
      array(
        'data' => t('There are no store types yet. <a href="@link">Add store type</a>.', array('@link' => url('admin/commerce/stores/types/add'))),
        'colspan' => 2,
      )
    );
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

/**
 * Builds an overview of a store type for display to an administrator.
 *
 * @param $variables
 *   An array of variables used to generate the display; by default includes the
 *     type key with a value of the store type array.
 *
 * @ingroup themeable
 */
function theme_store_type_admin_overview($variables) {
  $store_type = $variables['store_type'];

  $output = check_plain($store_type['name']);
  $output .= ' <small>' . t('(Machine name: @type)', array('@type' => $store_type['type'])) . '</small>';
  $output .= '<div class="description">' . filter_xss_admin($store_type['description']) . '</div>';

  return $output;
}

/**
 * Form callback wrapper: create or edit a store type.
 *
 * @param $type
 *   The machine-name of the store type being created or edited by this form
 *     or a full store type array.
 *
 * @see commerce_store_store_type_form()
 */
function commerce_store_ui_store_type_form_wrapper($type) {
  if (is_array($type)) {
    $store_type = $type;
  }
  else {
    $store_type = commerce_store_type_load($type);
  }

  // Return a message if the store type is not governed by Store UI.
  if (!empty($store_type['type']) && $store_type['module'] != 'commerce_store_ui') {
    return t('This store type cannot be edited, because it is not defined by the Store UI module.');
  }

  // Include the forms file from the Store module.
  module_load_include('inc', 'commerce_store_ui', 'includes/commerce_store_ui.forms');

  return drupal_get_form('commerce_store_ui_store_type_form', $store_type);
}

/**
 * Form callback wrapper: confirmation form for deleting a store type.
 *
 * @param $type
 *   The machine-name of the store type being created or edited by this form
 *     or a full store type array.
 *
 * @see commerce_store_store_type_delete_form()
 */
function commerce_store_ui_store_type_delete_form_wrapper($type) {
  if (is_array($type)) {
    $store_type = $type;
  }
  else {
    $store_type = commerce_store_type_load($type);
  }

  // Return a message if the store type is not governed by Store UI.
  if ($store_type['module'] != 'commerce_store_ui') {
    return t('This store type cannot be deleted, because it is not defined by the Store UI module.');
  }

  // Don't allow deletion of store types that have stores already.
  $query = new EntityFieldQuery();

  $query->entityCondition('entity_type', 'commerce_store', '=')
    ->entityCondition('bundle', $store_type['type'], '=')
    ->count();

  $count = $query->execute();

  if ($count > 0) {
    drupal_set_title(t('Cannot delete the %name store type', array('%name' => $store_type['name'])), PASS_THROUGH);

    return format_plural($count,
      'There is 1 store of this type. It cannot be deleted.',
      'There are @count stores of this type. It cannot be deleted.'
    );
  }

  // Include the forms file from the Store module.
  module_load_include('inc', 'commerce_store_ui', 'includes/commerce_store_ui.forms');

  return drupal_get_form('commerce_store_ui_store_type_delete_form', $store_type);
}
