<?php

/**
 * Field handler to present a link to a store.
 */
class commerce_store_ui_handler_field_store_link extends views_handler_field {
  function construct() {
    parent::construct();

    $this->additional_fields['store_id'] = 'store_id';
  }

  function option_definition() {
    $options = parent::option_definition();

    $options['text'] = array('default' => '', 'translatable' => TRUE);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['text'] = array(
      '#type' => 'textfield',
      '#title' => t('Text to display'),
      '#default_value' => $this->options['text'],
    );
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    $text = !empty($this->options['text']) ? $this->options['text'] : t('view');
    $store_id = $this->get_value($values, 'store_id');

    return l($text, 'store/' . $store_id);
  }
}
