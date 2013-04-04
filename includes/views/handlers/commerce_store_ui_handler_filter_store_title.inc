<?php

/**
 * Filter by order status.
 */
class commerce_store_ui_handler_filter_store_title extends views_handler_filter_in_operator {
  function get_value_options() {
    global $user;
    if (!isset($this->value_options)) {
      $this->value_title = t('Store');
      $this->value_options = array();
      foreach (commerce_store_load_multiple(NULL, array('uid' => $user->uid)) as $store_id => $store) {
        $this->value_options[$store->title] = $store->title;
      }
    }
  }
}
