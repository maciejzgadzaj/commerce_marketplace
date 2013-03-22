<?php

/**
 * Filter by store type.
 */
class commerce_store_ui_handler_filter_store_type extends views_handler_filter_in_operator {
  // Display a list of store types in the filter's options.
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Store type');
      $this->value_options = commerce_store_type_get_name();
    }
  }
}
