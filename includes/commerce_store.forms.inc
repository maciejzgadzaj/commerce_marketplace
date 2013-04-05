<?php

/**
 * @file
 * Forms for creating, editing, and deleting stores.
 */


/**
 * Form callback: create or edit a store.
 *
 * @param $store
 *   The store object to edit or for a create form an empty store object
 *     with only a store type defined.
 */
function commerce_store_store_form($form, &$form_state, $store) {
  global $user;

  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'commerce_store') . '/includes/commerce_store.forms.inc';

  // Add the default field elements.
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => $store->title,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -5,
  );

  // Add the field related form elements.
  $form_state['commerce_store'] = $store;

  $langcode = entity_language('commerce_store', $store);

  if (empty($langcode)) {
    $langcode = LANGUAGE_NONE;
  }

  field_attach_form('commerce_store', $store, $form, $form_state, $langcode);

  $form['additional_settings'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 99,
  );

  // Store owner information.
  $owner = user_load($store->uid);
  $form['owner'] = array(
    '#type' => 'fieldset',
    '#access' => user_access('administer stores'),
    '#title' => t('Store owner'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
    '#attributes' => array(
      'class' => array('store-form-owner'),
    ),
    '#attached' => array(
      'js' => array(
        drupal_get_path('module', 'commerce_store_ui') . '/commerce_store_ui.js',
      ),
    ),
    '#weight' => 20,
  );
  $form['owner']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Store owner'),
    '#maxlength' => 60,
    '#autocomplete_path' => 'user/autocomplete',
    '#default_value' => !empty($owner->name) ? $owner->name : '',
    '#weight' => -1,
    '#description' => t('Owner of the store will have full control over the store, its orders and customers.'),
  );

  // Load the store type to get the default revision setting.
  $store_type = commerce_store_type_load($store->type);

  // When updating a store, do not collapse the Change History fieldset if the
  // store type is configured to create a new revision by default.
  $form['revision_information'] = array(
    '#type' => 'fieldset',
    '#title' => t('Revision information'),
    '#collapsible' => TRUE,
    '#collapsed' => empty($store->store_id) || empty($store_type['revision']),
    '#group' => 'additional_settings',
    '#attributes' => array(
      'class' => array('store-form-revision-information'),
    ),
    '#weight' => 50,
  );
  if (!empty($store->store_id)) {
    $form['revision_information']['revision'] = array(
      '#type' => 'checkbox',
      '#title' => t('Create new revision'),
      '#description' => t('If an update log message is entered, a revision will be created even if this is unchecked.'),
      '#default_value' => $store_type['revision'],
      '#access' => user_access('administer commerce_store entities'),
    );
    // Check the revision log checkbox when the log textarea is filled in.
    // This must not happen if "Create new revision" is enabled by default, since
    // the state would auto-disable the checkbox otherwise.
    if (empty($store->revision)) {
      $form['revision_information']['revision']['#states'] = array(
        'checked' => array(
          'textarea[name="log"]' => array('empty' => FALSE),
        ),
      );
    }
  }
  $form['revision_information']['log'] = array(
    '#type' => 'textarea',
    '#title' => !empty($store->store_id) ? t('Update log message') : t('Creation log message'),
    '#rows' => 4,
    '#description' => t('Provide an explanation of the changes you are making. This will provide a meaningful history of changes to this store.'),
  );

  // Node options for administrators
  $form['options'] = array(
    '#type' => 'fieldset',
    '#access' => user_access('administer stores'),
    '#title' => t('Status options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
    '#attributes' => array(
      'class' => array('store-form-options'),
    ),
    '#attached' => array(
      'js' => array(drupal_get_path('module', 'commerce_store_ui') . '/commerce_store_ui.js'),
    ),
    '#weight' => 95,
  );
  $form['options']['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Active'),
    '#default_value' => $store->status,
  );

  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight' => 400,
  );

  // Simply use default language
  $form['language'] = array(
    '#type' => 'value',
    '#value' => $langcode,
  );

  // We add the form's #submit array to this button along with the actual submit
  // handler to preserve any submit handlers added by a form callback_wrapper.
  $submit = array();

  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save store'),
    '#submit' => array_merge($submit, array('commerce_store_store_form_submit')),
  );

  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'commerce_store_store_form_validate';

  return $form;
}

/**
 * Validation callback for commerce_store_store_form().
 */
function commerce_store_store_form_validate($form, &$form_state) {
  $store = $form_state['commerce_store'];

  // Validate the "Store owner" field. Empty value is fine too,
  // as in such a case we just use current user.
  if (!empty($form_state['values']['name']) && !($account = user_load_by_name($form_state['values']['name']))) {
    form_set_error('name', t('The username %name does not exist.', array('%name' => $form_state['values']['name'])));
  }

  // Notify field widgets to validate their data.
  field_attach_form_validate('commerce_store', $store, $form, $form_state);
}

/**
 * Submit callback for commerce_store_store_form().
 */
function commerce_store_store_form_submit($form, &$form_state) {
  global $user;

  $store = &$form_state['commerce_store'];

  // Save default parameters back into the $store object.
  $store->title = $form_state['values']['title'];
  $store->status = $form_state['values']['status'];
  $store->language = $form_state['values']['language'];

  // If no store owner was provided in the form, use current user.
  if (!empty($form_state['values']['name'])) {
    $account = user_load_by_name($form_state['values']['name']);
    $store->uid = $account->uid;
  }
  else {
    $store->uid = $user->uid;
  }

  // Trigger a new revision if the checkbox was enabled or a log message supplied.
  if ((user_access('administer commerce_store entities') && !empty($form_state['values']['revision'])) ||
    (!user_access('administer commerce_store entities') && !empty($form['change_history']['revision']['#default_value'])) ||
    !empty($form_state['values']['log'])) {
    $store->revision = TRUE;
    $store->log = $form_state['values']['log'];
  }

  // Notify field widgets.
  field_attach_submit('commerce_store', $store, $form, $form_state);

  // Save the store.
  commerce_store_save($store);

  // Redirect based on the button clicked.
  drupal_set_message(t('Store saved.'));
}

/**
 * Form callback: confirmation form for deleting a store.
 *
 * @param $store
 *   The store object to be deleted.
 *
 * @see confirm_form()
 */
function commerce_store_store_delete_form($form, &$form_state, $store) {
  $form_state['store'] = $store;

  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'commerce_store') . '/includes/commerce_store.forms.inc';

  $form['#submit'][] = 'commerce_store_store_delete_form_submit';

  $content = entity_view('commerce_store', array($store->store_id => $store));

  $form = confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $store->title)),
    '',
    drupal_render($content) . '<p>' . t('Deleting this store cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for commerce_store_store_delete_form().
 */
function commerce_store_store_delete_form_submit($form, &$form_state) {
  $store = $form_state['store'];

  if (commerce_store_delete($store->store_id)) {
    drupal_set_message(t('%title has been deleted.', array('%title' => $store->title)));
    watchdog('commerce_store', 'Deleted store %title.', array('%title' => $store->title), WATCHDOG_NOTICE);
  }
  else {
    drupal_set_message(t('%title could not be deleted.', array('%title' => $store->title)), 'error');
  }
}
