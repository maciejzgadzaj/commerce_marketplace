<?php

/**
 * @file
 * Provides metadata for the store entity.
 */

/**
 * Implements hook_entity_property_info().
 */
function commerce_store_entity_property_info() {
  $info = array();

  // Add meta-data about the basic commerce_store properties.
  $properties = &$info['commerce_store']['properties'];

  $properties['store_id'] = array(
    'label' => t('Store ID'),
    'description' => t('The internal numeric ID of the store.'),
    'type' => 'integer',
    'schema field' => 'store_id',
  );
  $properties['type'] = array(
    'label' => t('Type'),
    'description' => t('The type of the store.'),
    'type' => 'token',
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer commerce_store entities',
    'options list' => 'commerce_store_type_options_list',
    'required' => TRUE,
    'schema field' => 'type',
  );
  $properties['title'] = array(
    'label' => t('Title'),
    'description' => t('The title of the store.'),
    'type' => 'text',
    'setter callback' => 'entity_property_verbatim_set',
    'required' => TRUE,
    'schema field' => 'title',
  );
  $properties['language'] = array(
    'label' => t('Language'),
    'type' => 'token',
    'description' => t('The language the store was created in.'),
    'setter callback' => 'entity_property_verbatim_set',
    'options list' => 'entity_metadata_language_list',
    'schema field' => 'language',
    'setter permission' => 'administer commerce_store entities',
  );
  $properties['status'] = array(
    'label' => t('Status'),
    'description' => t('Boolean indicating whether the store is active or disabled.'),
    'type' => 'boolean',
    'options list' => 'commerce_store_status_options_list',
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer commerce_store entities',
    'schema field' => 'status',
  );
  $properties['created'] = array(
    'label' => t('Date created'),
    'description' => t('The date the store was created.'),
    'type' => 'date',
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer commerce_store entities',
    'schema field' => 'created',
  );
  $properties['changed'] = array(
    'label' => t('Date updated'),
    'description' => t('The date the store was most recently updated.'),
    'type' => 'date',
    'setter callback' => 'entity_property_verbatim_set',
    'query callback' => 'entity_metadata_table_query',
    'setter permission' => 'administer commerce_store entities',
    'schema field' => 'changed',
  );
  $properties['uid'] = array(
    'label' => t('Creator ID'),
    'type' => 'integer',
    'description' => t('The unique ID of the store creator.'),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer commerce_store entities',
    'clear' => array('creator'),
    'schema field' => 'uid',
  );
  $properties['creator'] = array(
    'label' => t('Creator'),
    'type' => 'user',
    'description' => t('The creator of the store.'),
    'getter callback' => 'commerce_store_get_properties',
    'setter callback' => 'commerce_store_set_properties',
    'setter permission' => 'administer commerce_store entities',
    'required' => TRUE,
    'computed' => TRUE,
    'clear' => array('uid'),
  );

  return $info;
}
