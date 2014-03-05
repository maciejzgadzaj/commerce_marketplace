<?php

/**
 * Form constructor for marketplace settings page.
 *
 * @see commerce_marketplace_checkout_menu()
 */
function commerce_marketplace_checkout_settings_form($form, &$form_state) {
  $defaults = commerce_marketplace_checkout_repeat_pages_defaults();
  $repeat_settings = variable_get('commerce_marketplace_checkout_repeat', $defaults);

  $options = array();
  foreach (commerce_checkout_pages() as $page_id => $page_details) {
    $options[$page_id] = $page_details['title'];
  }
  $form['commerce_marketplace_checkout_repeat'] = array(
    '#tree' => TRUE,
  );
  $form['commerce_marketplace_checkout_repeat']['pages'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Repeat checkout panes on following pages'),
    '#description' => t('@todo'),
    '#options' => $options,
    '#default_value' => $repeat_settings['pages'],
  );
  $form['commerce_marketplace_checkout_repeat']['titles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Repeat titles on following pages'),
    '#description' => t('@todo'),
    '#options' => $options,
    '#default_value' => $repeat_settings['titles'],
  );
  $form['commerce_marketplace_checkout_repeat']['buttons'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Repeat buttons on following pages'),
    '#description' => t('@todo'),
    '#options' => $options,
    '#default_value' => $repeat_settings['buttons'],
  );

  return system_settings_form($form);
}
