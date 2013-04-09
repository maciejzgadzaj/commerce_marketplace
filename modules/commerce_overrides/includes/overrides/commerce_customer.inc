<?php

/**
 * @file
 * Provides overrides for Commerce Customer functions.
 */

/**
 * Overrides commerce_customer_profile_pane_checkout_form_submit().
 *
 * Checkout pane callback: submits a customer profile edit form.
 */
function commerce_overrides_customer_profile_pane_checkout_form_submit($form, &$form_state, $checkout_pane, $order) {
  $order_group = $form_state['build_info']['args'][2];
  $orders = $form_state['build_info']['args'][3];
  $profile = $form_state['values'][$checkout_pane['pane_id']]['customer_profile'];

  // Ensure the profile is active.
  $profile->status = TRUE;

  // Set the profile's uid if it's being created at this time.
  if (empty($profile->profile_id)) {
    $profile->uid = $order->uid;
  }

  // Notify field widgets.
  field_attach_submit('commerce_customer_profile', $profile, $form[$checkout_pane['pane_id']], $form_state);

  // Save the profile.
  commerce_customer_profile_save($profile);

  // Store the profile ID for the related field as specified on the settings form.
  foreach ($orders as $order) {
    $wrapper = entity_metadata_wrapper('commerce_order', $order);

    if ($field_name = variable_get('commerce_' . $checkout_pane['pane_id'] . '_field', '')) {
      $wrapper->{$field_name} = $profile;
    }
    else {
      // Or make the association in the order's data array if no field was found.
      $order->data['profiles'][$checkout_pane['pane_id']] = $profile->profile_id;
    }
  }
}
