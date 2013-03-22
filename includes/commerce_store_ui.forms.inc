<?php

/**
 * @file
 * Forms for creating / editing and deleting stores.
 */


/**
 * Form callback: create or edit a store type.
 *
 * @param $store_type
 *   The store type array to edit or for a create form an empty store type
 *     array with properties instantiated but not populated.
 */
function commerce_store_ui_store_type_form($form, &$form_state, $store_type) {
  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'commerce_store_ui') . '/includes/commerce_store_ui.forms.inc';

  // Store the initial store type in the form state.
  $form_state['store_type'] = $store_type;

  $form['store_type'] = array(
    '#tree' => TRUE,
  );

  $form['store_type']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $store_type['name'],
    '#description' => t('The human-readable name of this store type. It is recommended that this name begin with a capital letter and contain only letters, numbers, and spaces. This name must be unique.'),
    '#required' => TRUE,
    '#size' => 32,
  );

  if (empty($store_type['type'])) {
    $form['store_type']['type'] = array(
      '#type' => 'machine_name',
      '#title' => t('Machine name'),
      '#default_value' => $store_type['type'],
      '#maxlength' => 32,
      '#required' => TRUE,
      '#machine_name' => array(
        'exists' => 'commerce_store_type_load',
        'source' => array('store_type', 'name'),
      ),
      '#description' => t('The machine-readable name of this store type. This name must contain only lowercase letters, numbers, and underscores, it must be unique.'),
    );
  }

  $form['store_type']['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('Describe this store type. The text will be displayed on the <em>Add new content</em> page.'),
    '#default_value' => $store_type['description'],
    '#rows' => 3,
  );

  $form['store_type']['help'] = array(
    '#type' => 'textarea',
    '#title' => t('Explanation or submission guidelines'),
    '#description' => t('This text will be displayed at the top of the page when creating or editing stores of this type.'),
    '#default_value' => $store_type['help'],
    '#rows' => 3,
  );

  $form['store_type']['revision'] = array(
    '#type' => 'checkbox',
    '#title' => t('Default stores of this type to be saved as new revisions when edited.'),
    '#default_value' => $store_type['revision'],
  );

  if (module_exists('entity_translation')) {
    $form['store_type']['multilingual'] = array(
      '#type' => 'radios',
      '#title' => t('Multilingual support'),
      '#description' => t('If <em>Entity translation</em> is enabled it will be possible to provide a different version of the same store for each available language.') . '<br />' . t('You can find more options in the <a href="!url">entity translation settings</a>.', array('!url' => url('admin/config/regional/entity_translation'))) . '<br />' . t('Existing stores will not be affected by changing this option.'),
      '#options' => array(
        0 => t('Disabled'),
        ENTITY_TRANSLATION_ENABLED => t('Enabled via <em>Entity translation</em>'),
      ),
      '#default_value' => variable_get('language_store_type_' . $store_type['type'], 0),
    );
  }

  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight' => 40,
  );

  // We add the form's #submit array to this button along with the actual submit
  // handler to preserve any submit handlers added by a form callback_wrapper.
  $submit = array();

  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save store type'),
    '#submit' => array_merge($submit, array('commerce_store_ui_store_type_form_submit')),
  );

  if (!empty($form_state['store_type']['type'])) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete store type'),
      '#suffix' => l(t('Cancel'), 'admin/commerce/stores/types'),
      '#submit' => array_merge($submit, array('commerce_store_ui_store_type_form_delete_submit')),
      '#weight' => 45,
    );
  }
  else {
    $form['actions']['save_continue'] = array(
      '#type' => 'submit',
      '#value' => t('Save and add fields'),
      '#suffix' => l(t('Cancel'), 'admin/commerce/stores/types'),
      '#submit' => array_merge($submit, array('commerce_store_ui_store_type_form_submit')),
      '#weight' => 45,
    );
  }

  $form['#validate'][] = 'commerce_store_ui_store_type_form_validate';

  return $form;
}

/**
 * Validation callback for commerce_store_store_type_form().
 */
function commerce_store_ui_store_type_form_validate($form, &$form_state) {
  $store_type = $form_state['store_type'];

  // If saving a new store type, ensure it has a unique machine name.
  if (empty($store_type['type'])) {
    if (!commerce_store_ui_validate_store_type_unique($form_state['values']['store_type']['type'])) {
      form_set_error('store_type][type', t('The machine name specified is already in use.'));
    }
  }
}

/**
 * Form submit handler: save a store type.
 */
function commerce_store_ui_store_type_form_submit($form, &$form_state) {
  $store_type = $form_state['store_type'];
  $updated = !empty($store_type['type']);

  // If a type is set, we should still check to see if a row for the type exists
  // in the database; this is done to accomodate types defined by Features.
  if ($updated) {
    $updated = db_query('SELECT 1 FROM {commerce_store_type} WHERE type = :type', array(':type' => $store_type['type']))->fetchField();
  }

  foreach ($form_state['values']['store_type'] as $key => $value) {
    $store_type[$key] = $value;
  }

  // Write the store type to the database.
  $store_type['is_new'] = !$updated;
  commerce_store_ui_store_type_save($store_type);

  // Set the multingual value for the store type if entity translation is enabled.
  if (module_exists('entity_translation')) {
    variable_set('language_store_type_' . $store_type['type'], $store_type['multilingual']);
  }

  // Redirect based on the button clicked.
  drupal_set_message(t('Store type saved.'));

  if ($form_state['triggering_element']['#parents'][0] == 'save_continue') {
    $form_state['redirect'] = 'admin/commerce/stores/types/' . commerce_store_type_to_arg($store_type['type']) . '/fields';
  }
  else {
    $form_state['redirect'] = 'admin/commerce/stores/types';
  }
}

/**
 * Submit callback for delete button on commerce_store_ui_store_type_form().
 *
 * @see commerce_store_ui_store_type_form()
 */
function commerce_store_ui_store_type_form_delete_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/commerce/stores/types/' . commerce_store_type_to_arg($form_state['store_type']['type']) . '/delete';
}

/**
 * Form callback: confirmation form for deleting a store type.
 *
 * @param $store_type
 *   The store type array to be deleted.
 *
 * @see confirm_form()
 */
function commerce_store_ui_store_type_delete_form($form, &$form_state, $store_type) {
  $form_state['store_type'] = $store_type;

  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'commerce_store_ui') . '/includes/commerce_store_ui.forms.inc';

  $form['#submit'][] = 'commerce_store_ui_store_type_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete the %name store type?', array('%name' => $store_type['name'])),
    'admin/commerce/stores/types',
    '<p>' . t('This action cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for commerce_store_store_type_delete_form().
 */
function commerce_store_ui_store_type_delete_form_submit($form, &$form_state) {
  $store_type = $form_state['store_type'];

  commerce_store_ui_store_type_delete($store_type['type']);

  drupal_set_message(t('The store type %name has been deleted.', array('%name' => $store_type['name'])));
  watchdog('commerce_store', 'Deleted store type %name.', array('%name' => $store_type['name']), WATCHDOG_NOTICE);

  $form_state['redirect'] = 'admin/commerce/stores/types';
}
