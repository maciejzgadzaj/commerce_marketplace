<?php

/**
 * @file
 * Contains the basic store field handler.
 */

/**
 * Field handler to provide simple renderer that allows linking to a store.
 */
class commerce_store_ui_handler_field_store extends views_handler_field {
  function init(&$view, &$options) {
    parent::init($view, $options);

    if (!empty($this->options['link_to_store'])) {
      $this->additional_fields['store_id'] = 'store_id';
    }
  }

  function option_definition() {
    $options = parent::option_definition();

    $options['link_to_store'] = array('default' => FALSE);

    return $options;
  }

  /**
   * Provide the link to store option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['link_to_store'] = array(
      '#title' => t("Link this field to the store's administrative view page"),
      '#description' => t('This will override any other link you have set.'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['link_to_store']),
    );
  }

  /**
   * Render whatever the data is as a link to the store.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($data, $values) {
    if (!empty($this->options['link_to_store']) && $data !== NULL && $data !== '') {
      $store_id = $this->get_value($values, 'store_id');
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = 'stores/' . $store_id;
    }

    return $data;
  }

  function render($values) {
    $value = $this->get_value($values);
    return $this->render_link($this->sanitize_value($value), $values);
  }
}
