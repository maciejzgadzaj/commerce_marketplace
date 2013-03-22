<?php

/**
 * Argument handler to display store titles in View using store arguments.
 */
class commerce_store_ui_handler_argument_store_id extends views_handler_argument_numeric {
  function title_query() {
    $titles = array();
    $result = db_select('commerce_store', 'cs')
      ->fields('cs', array('title'))
      ->condition('cs.store_id', $this->value)
      ->execute();
    foreach ($result as $store) {
      $titles[] = check_plain($store->title);
    }
    return $titles;
  }
}
