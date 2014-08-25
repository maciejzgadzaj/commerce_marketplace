<?php

/**
 * @file
 * Forms for creating / editing and deleting stores.
 */


/**
 * Form constructor for edit a store type form.
 *
 * @param array $store_type
 *   The store type array to edit or for a create form an empty store type
 *   array with properties instantiated but not populated.
 *
 * @see commerce_store_ui_store_type_form_validate()
 * @see commerce_store_ui_store_type_form_submit()
 * @see commerce_store_ui_store_type_form_delete_submit()
 *
 * @ingroup forms
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
    '#title' => t('By default stores of this type to be saved as new revisions when edited.'),
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
 * Form validation handler for commerce_store_store_type_form().
 *
 * @see commerce_store_ui_store_type_form_submit()
 * @see commerce_store_ui_store_type_form_delete_submit()
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
 * Form submission handler for commerce_store_ui_store_type_form().
 *
 * @see commerce_store_ui_store_type_form_validate()
 * @see commerce_store_ui_store_type_form_delete_submit()
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
 * Form submission handler for commerce_store_ui_store_type_form().
 *
 * @see commerce_store_ui_store_type_form_validate()
 * @see commerce_store_ui_store_type_form_submit()
 */
function commerce_store_ui_store_type_form_delete_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/commerce/stores/types/' . commerce_store_type_to_arg($form_state['store_type']['type']) . '/delete';
}

/**
 * Form constructor for confirmation form for deleting a store type.
 *
 * @param array $store_type
 *   The store type array to be deleted.
 *
 * @see confirm_form()
 * @see commerce_store_ui_store_type_delete_form_submit()
 *
 * @ingroup forms
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
 * Form submission handler for commerce_store_store_type_delete_form().
 */
function commerce_store_ui_store_type_delete_form_submit($form, &$form_state) {
  $store_type = $form_state['store_type'];

  commerce_store_ui_store_type_delete($store_type['type']);

  drupal_set_message(t('The store type %name has been deleted.', array('%name' => $store_type['name'])));
  watchdog('commerce_store', 'Deleted store type %name.', array('%name' => $store_type['name']), WATCHDOG_NOTICE);

  $form_state['redirect'] = 'admin/commerce/stores/types';
}

/**
 * Form constructor for store edit form.
 *
 * @param object $store
 *   The store object to edit or for a create form an empty store object
 *   with only a store type defined.
 *
 * @see commerce_store_ui_store_form_validate()
 * @see commerce_store_ui_store_form_submit()
 *
 * @ingroup forms
 */
function commerce_store_ui_store_form($form, &$form_state, $store) {
  global $user;

  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'commerce_store') . '/includes/commerce_store_ui.forms.inc';

  $form['#tree'] = TRUE;

  // Add the default field elements.
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => $store->title,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -5,
  );

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
    '#access' => user_access('administer commerce_store entities'),
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
    '#description' => t('Owner of the store will have full control over the store, its orders and customers. Leave empty to use current user.'),
  );

  // Load the store type to get the default revision setting.
  $store_type = commerce_store_type_load($store->type);

  // When updating a store, do not collapse the Change History fieldset if the
  // store type is configured to create a new revision by default.
  $form['revision_information'] = array(
    '#type' => 'fieldset',
    '#title' => t('Change history'),
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
          'textarea[name="revision_information[log]"]' => array('empty' => FALSE),
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

  // Store options for administrators.
  $form['options'] = array(
    '#type' => 'fieldset',
    '#access' => empty($store->store_id) || commerce_store_access('update', $store),
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
    '#weight' => 40,
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

  // Simply use default language.
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
    '#submit' => array_merge($submit, array('commerce_store_ui_store_form_submit')),
  );

  // Form submit/cancel redirect depends on user access rights.
  // We will need it in submit callback as well, let's then store it in form.
  $form['#redirect'] = user_access('administer commerce_store entities') ? 'admin/commerce/stores' : 'user/' . $user->uid . '/merchant/stores';

  // Add the 'Save and add another' button for new stores for admin users.
  if (empty($store->store_id) && user_access('administer commerce_store entities')) {
    $form['actions']['save_continue'] = array(
      '#type' => 'submit',
      '#value' => t('Save and add another'),
      '#submit' => $form['actions']['submit']['#submit'],
      '#suffix' => l(t('Cancel'), $form['#redirect']),
      '#weight' => 45,
    );
  }
  else {
    $form['actions']['submit']['#suffix'] = l(t('Cancel'), $form['#redirect']);
  }

  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'commerce_store_ui_store_form_validate';

  return $form;
}

/**
 * Form validation handler for commerce_store_ui_store_form().
 *
 * @see commerce_store_ui_store_form_submit()
 */
function commerce_store_ui_store_form_validate($form, &$form_state) {
  $store = $form_state['build_info']['args'][0];

  // Validate the "Store owner" field. Empty value is fine too,
  // as in such a case we just use current user.
  if (!empty($form_state['values']['owner']['name']) && !($account = user_load_by_name($form_state['values']['owner']['name']))) {
    form_set_error('name', t('The username %name does not exist.', array('%name' => $form_state['values']['owner']['name'])));
  }

  // Notify field widgets to validate their data.
  field_attach_form_validate('commerce_store', $store, $form, $form_state);
}

/**
 * Form submission handler for commerce_store_ui_store_form().
 *
 * @see commerce_store_ui_store_form_validate()
 */
function commerce_store_ui_store_form_submit($form, &$form_state) {
  global $user;

  $store = $form_state['build_info']['args'][0];

  // Save default parameters back into the $store object.
  $store->title = $form_state['values']['title'];
  $store->status = $form_state['values']['options']['status'];
  $store->language = $form_state['values']['language'];

  // If no store owner was provided in the form, use current user.
  if (!empty($form_state['values']['owner']['name'])) {
    $account = user_load_by_name($form_state['values']['owner']['name']);
  }
  else {
    $account = $user;
  }
  $store->uid = $account->uid;

  // Trigger a new revision if the checkbox was enabled or a log message supplied.
  if (
    (user_access('administer commerce_store entities') && !empty($form_state['values']['revision_information']['revision']))
    || (!user_access('administer commerce_store entities') && !empty($form['change_history']['revision']['#default_value']))
    || !empty($form_state['values']['revision_information']['log'])
  ) {
    $store->revision = TRUE;
    $store->log = $form_state['values']['revision_information']['log'];
  }

  // Notify field widgets.
  field_attach_submit('commerce_store', $store, $form, $form_state);

  // Save the store.
  commerce_store_save($store);
  drupal_set_message(t('Store saved.'));

  // Add "merchant" role to user owning the store.
  if ($merchant_role = user_role_load_by_name(COMMERCE_STORE_MERCHANT)) {
    if (empty($account->roles[$merchant_role->rid])) {
      $edit = array('roles' => $account->roles);
      $edit['roles'][$merchant_role->rid] = $merchant_role->name;
      user_save($account, $edit);
    }
  }

  // TODO: Do we want to remove "merchant" role from previous store owner?
  // When store owner is being changed, it might happen that previous store
  // owner (which we have stored in $form_state['commerce_store']) no longer
  // owns any store - should we remove "merchant" role from such user then?

  // Set the redirect based on the button clicked.
  $array_parents = $form_state['triggering_element']['#array_parents'];
  $submit_element = array_pop($array_parents);

  if ($submit_element == 'save_continue') {
    $form_state['redirect'] = 'store/add/' . commerce_store_type_to_arg($store->type);
  }
  else {
    $form_state['redirect'] = $form['#redirect'];
  }
}

/**
 * Form callback: confirmation form for deleting a store.
 *
 * @param $store
 *   The store object to be deleted.
 *
 * @see confirm_form()
 */
function commerce_store_ui_store_delete_form($form, &$form_state, $store) {
  $form_state['store'] = $store;

  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'commerce_store') . '/includes/commerce_store_ui.forms.inc';

  $form['#submit'][] = 'commerce_store_ui_store_delete_form_submit';

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
 * Submit callback for commerce_store_ui_store_delete_form().
 */
function commerce_store_ui_store_delete_form_submit($form, &$form_state) {
  global $user;

  $store = $form_state['store'];

  if (commerce_store_delete($store->store_id)) {
    drupal_set_message(t('%title has been deleted.', array('%title' => $store->title)));
    watchdog('commerce_store', 'Deleted store %title.', array('%title' => $store->title), WATCHDOG_NOTICE);
  }
  else {
    drupal_set_message(t('%title could not be deleted.', array('%title' => $store->title)), 'error');
  }

  $form_state['redirect'] = user_access('administer commerce_store entities') ? 'admin/commerce/stores' : 'user/' . $user->uid . '/merchant/stores';
}
