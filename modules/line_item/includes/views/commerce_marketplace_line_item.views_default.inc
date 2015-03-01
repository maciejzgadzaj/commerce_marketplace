<?php

/**
 * Views for line item reference displays.
 */

/**
 * Implements hook_views_default_views_alter().
 *
 * Adds a relationship to Order entity to admin 'Line items' view.
 *
 * @see commerce_line_item_views_default_views()
 */
function commerce_marketplace_line_item_views_default_views_alter(&$views) {
  if (isset($views['commerce_line_item_table'])) {
    $view = &$views['commerce_line_item_table'];
    /* Display: Defaults */
    $handler = $view->display['default']->handler;
    /* Relationship: Commerce Line Item: Order ID */
    $handler->display->display_options['relationships']['order_id']['id'] = 'order_id';
    $handler->display->display_options['relationships']['order_id']['table'] = 'commerce_line_item';
    $handler->display->display_options['relationships']['order_id']['field'] = 'order_id';
    $handler->display->display_options['relationships']['order_id']['required'] = TRUE;
  }
}
