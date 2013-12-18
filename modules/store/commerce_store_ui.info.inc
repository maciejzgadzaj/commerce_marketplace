<?php

/**
 * Implements hook_entity_property_info_alter().
 *
 * Add UI bound store properties.
 */
function commerce_store_ui_entity_property_info_alter(&$info) {
  $info['commerce_store']['properties']['edit_url'] = array(
    'label' => t('Edit URL'),
    'description' => t("The URL of the store's edit page."),
    'getter callback' => 'commerce_store_get_properties',
    'type' => 'uri',
  );
}
