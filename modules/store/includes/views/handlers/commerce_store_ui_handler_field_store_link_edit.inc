<?php

/**
 * Field handler to present a store edit link.
 */
class commerce_store_ui_handler_field_store_link_edit extends commerce_store_ui_handler_field_store_link {
  function construct() {
    parent::construct();

    $this->additional_fields['type'] = 'type';
    $this->additional_fields['uid'] = 'uid';
  }

  function render($values) {
    // Ensure the user has access to edit this store.
    $store = commerce_store_new();
    $store->store_id = $this->get_value($values, 'store_id');
    $store->type = $this->get_value($values, 'type');
    $store->uid = $this->get_value($values, 'uid');

    if (!commerce_store_access('update', $store)) {
      return;
    }

    $text = !empty($this->options['text']) ? $this->options['text'] : t('edit');

    return l($text, 'admin/commerce/stores/' . $store->store_id . '/edit', array('query' => drupal_get_destination()));
  }
}
