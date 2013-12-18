<?php

/**
 * @file
 * Overrides for Drupal Commerce views.
 */

/**
 * Implements hook_views_default_views_alter().
 */
function commerce_marketplace_checkout_views_default_views_alter(&$views) {

  // Overrides for "Checkout" cart view.
  if (isset($views['commerce_cart_summary'])) {
    // Allow multiple values (order IDs) in argument.
    $views['commerce_cart_summary']->display['default']->display_options['arguments']['order_id']['break_phrase'] = TRUE;
  }

}
