<?php

/**
 * Expose Drupal Commerce stores to Views.
 */

/**
 * Implements hook_views_data()
 */
function commerce_store_ui_views_data() {
  $data = array();

  $data['commerce_store']['table']['group']  = t('Commerce Store');

  $data['commerce_store']['table']['base'] = array(
    'field' => 'store_id',
    'title' => t('Commerce Store'),
    'help' => t('Drupal Commerce stores from the store.'),
    'access query tag' => 'commerce_store_access',
  );
  $data['commerce_store']['table']['entity type'] = 'commerce_store';

  $data['commerce_store']['table']['default_relationship'] = array(
    'commerce_store_revision' => array(
      'table' => 'commerce_store_revision',
      'field' => 'revision_id',
    ),
  );

  // Expose the store ID.
  $data['commerce_store']['store_id'] = array(
    'title' => t('Store ID'),
    'help' => t('The unique internal identifier of the store.'),
    'field' => array(
      'handler' => 'commerce_store_ui_handler_field_store',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'commerce_store_ui_handler_argument_store_id',
    ),
  );

  // Expose the store type.
  $data['commerce_store']['type'] = array(
    'title' => t('Type'),
    'help' => t('The human-readable name of the type of the store.'),
    'field' => array(
      'handler' => 'commerce_store_ui_handler_field_store_type',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'commerce_store_ui_handler_filter_store_type',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Expose the store title.
  $data['commerce_store']['title'] = array(
    'title' => t('Title'),
    'help' => t('The title of the store used for administrative display.'),
    'field' => array(
      'handler' => 'commerce_store_ui_handler_field_store',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  if (module_exists('locale')) {
    // Expose the language
    $data['commerce_store']['language'] = array(
      'title' => t('Language'),
      'help' => t('The language the store is in.'),
      'field' => array(
       'handler' => 'views_handler_field_locale_language',
       'click sortable' => TRUE,
      ),
     'filter' => array(
       'handler' => 'views_handler_filter_locale_language',
     ),
     'argument' => array(
       'handler' => 'views_handler_argument_locale_language',
     ),
     'sort' => array(
     'handler' => 'views_handler_sort',
     ),
    );
  }

  // Expose the owner uid.
  $data['commerce_store']['uid'] = array(
    'title' => t('Uid'),
    'help' => t("The owner's user ID."),
    'field' => array(
      'handler' => 'views_handler_field_user',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_user_uid',
      'name field' => 'name', // display this field in the summary
    ),
    'filter' => array(
      'title' => t('Name'),
      'handler' => 'views_handler_filter_user_name',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'relationship' => array(
      'title' => t('Owner'),
      'help' => t("Relate this store to its owner's user account"),
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'base field' => 'uid',
      'field' => 'uid',
      'label' => t('Store owner'),
    ),
  );

  // Expose the store status.
  $data['commerce_store']['status'] = array(
    'title' => t('Status'),
    'help' => t('Whether or not the store is active.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
      'output formats' => array(
        'active-disabled' => array(t('Active'), t('Disabled')),
      ),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Active'),
      'type' => 'yes-no',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Expose the created and changed timestamps.
  $data['commerce_store']['created'] = array(
    'title' => t('Created date'),
    'help' => t('The date the store was created.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  $data['commerce_store']['created_fulldate'] = array(
    'title' => t('Created date'),
    'help' => t('In the form of CCYYMMDD.'),
    'argument' => array(
      'field' => 'created',
      'handler' => 'views_handler_argument_node_created_fulldate',
    ),
  );

  $data['commerce_store']['created_year_month'] = array(
    'title' => t('Created year + month'),
    'help' => t('In the form of YYYYMM.'),
    'argument' => array(
      'field' => 'created',
      'handler' => 'views_handler_argument_node_created_year_month',
    ),
  );

  $data['commerce_store']['created_timestamp_year'] = array(
    'title' => t('Created year'),
    'help' => t('In the form of YYYY.'),
    'argument' => array(
      'field' => 'created',
      'handler' => 'views_handler_argument_node_created_year',
    ),
  );

  $data['commerce_store']['created_month'] = array(
    'title' => t('Created month'),
    'help' => t('In the form of MM (01 - 12).'),
    'argument' => array(
      'field' => 'created',
      'handler' => 'views_handler_argument_node_created_month',
    ),
  );

  $data['commerce_store']['created_day'] = array(
    'title' => t('Created day'),
    'help' => t('In the form of DD (01 - 31).'),
    'argument' => array(
      'field' => 'created',
      'handler' => 'views_handler_argument_node_created_day',
    ),
  );

  $data['commerce_store']['created_week'] = array(
    'title' => t('Created week'),
    'help' => t('In the form of WW (01 - 53).'),
    'argument' => array(
      'field' => 'created',
      'handler' => 'views_handler_argument_node_created_week',
    ),
  );

  $data['commerce_store']['changed'] = array(
    'title' => t('Updated date'),
    'help' => t('The date the store was last updated.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  $data['commerce_store']['changed_fulldate'] = array(
    'title' => t('Updated date'),
    'help' => t('In the form of CCYYMMDD.'),
    'argument' => array(
      'field' => 'changed',
      'handler' => 'views_handler_argument_node_created_fulldate',
    ),
  );

  $data['commerce_store']['changed_year_month'] = array(
    'title' => t('Updated year + month'),
    'help' => t('In the form of YYYYMM.'),
    'argument' => array(
      'field' => 'changed',
      'handler' => 'views_handler_argument_node_created_year_month',
    ),
  );

  $data['commerce_store']['changed_timestamp_year'] = array(
    'title' => t('Updated year'),
    'help' => t('In the form of YYYY.'),
    'argument' => array(
      'field' => 'changed',
      'handler' => 'views_handler_argument_node_created_year',
    ),
  );

  $data['commerce_store']['changed_month'] = array(
    'title' => t('Updated month'),
    'help' => t('In the form of MM (01 - 12).'),
    'argument' => array(
      'field' => 'changed',
      'handler' => 'views_handler_argument_node_created_month',
    ),
  );

  $data['commerce_store']['changed_day'] = array(
    'title' => t('Updated day'),
    'help' => t('In the form of DD (01 - 31).'),
    'argument' => array(
      'field' => 'changed',
      'handler' => 'views_handler_argument_node_created_day',
    ),
  );

  $data['commerce_store']['changed_week'] = array(
    'title' => t('Updated week'),
    'help' => t('In the form of WW (01 - 53).'),
    'argument' => array(
      'field' => 'changed',
      'handler' => 'views_handler_argument_node_created_week',
    ),
  );

  // Expose links to operate on the store.
  $data['commerce_store']['view_store'] = array(
    'field' => array(
      'title' => t('Link'),
      'help' => t('Provide a simple link to the view page of the store.'),
      'handler' => 'commerce_store_ui_handler_field_store_link',
    ),
  );
  $data['commerce_store']['edit_store'] = array(
    'field' => array(
      'title' => t('Edit link'),
      'help' => t('Provide a simple link to edit the store.'),
      'handler' => 'commerce_store_ui_handler_field_store_link_edit',
    ),
  );
  $data['commerce_store']['delete_store'] = array(
    'field' => array(
      'title' => t('Delete link'),
      'help' => t('Provide a simple link to delete the store.'),
      'handler' => 'commerce_store_ui_handler_field_store_link_delete',
    ),
  );

  $data['commerce_store']['operations'] = array(
    'field' => array(
      'title' => t('Operations links'),
      'help' => t('Display all the available operations links for the store.'),
      'handler' => 'commerce_store_ui_handler_field_store_operations',
    ),
  );

  $data['commerce_store']['empty_text'] = array(
    'title' => t('Empty text'),
    'help' => t('Displays an appropriate empty text message for store lists.'),
    'area' => array(
      'handler' => 'commerce_store_ui_handler_area_empty_text',
    ),
  );

  /**
   * Integrate the store revision table.
   */
  $data['commerce_store_revision']['table']['entity type'] = 'commerce_store';
  $data['commerce_store_revision']['table']['group']  = t('Commerce Store revision');

  // Advertise this table as a possible base table.
  $data['commerce_store_revision']['table']['base'] = array(
    'field' => 'revision_id',
    'title' => t('Commerce Store revision'),
    'help' => t('Commerce Store revision is a history of changes to a store.'),
    'defaults' => array(
      'field' => 'title',
    ),
  );

  $data['commerce_store_revision']['table']['join'] = array(
    'commerce_store' => array(
      'left_field' => 'revision_id',
      'field' => 'revision_id',
    )
  );

  $data['commerce_store_revision']['table']['default_relationship'] = array(
    'commerce_store' => array(
      'table' => 'commerce_store',
      'field' => 'revision_id',
    ),
  );

  // Expose the revision store ID
  $data['commerce_store_revision']['store_id'] = array(
    'title' => t('Store ID'),
    'help' => t('The unique internal identifier of the store.'),
    'field' => array(
      'handler' => 'commerce_store_ui_handler_field_store',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'commerce_store_ui_handler_argument_store_id',
    ),
  );

  // Expose the revision ID.
  $data['commerce_store_revision']['revision_id'] = array(
    'title' => t('Revision ID'),
    'help' => t('The revision ID of the store revision.'),
    'field' => array(
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'commerce_store',
      'base field' => 'revision_id',
      'title' => t('Store'),
      'label' => t('Latest store revision'),
    ),
  );

  // Expose the store revision title.
  $data['commerce_store_revision']['title'] = array(
    'title' => t('Title'),
    'help' => t('The title of the store revision used for administrative display.'),
    'field' => array(
      'handler' => 'commerce_store_ui_handler_field_store',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Expose the store revision user ID.
  $data['commerce_store_revision']['revision_uid'] = array(
    'title' => t('User'),
    'help' => t('Relate a store revision to the user who created the revision.'),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'base field' => 'uid',
      'field' => 'revision_uid',
      'field_name' => 'revision_uid',
      'label' => t('Revision user'),
    ),
  );

  // Expose the store revision status.
  $data['commerce_store_revision']['status'] = array(
    'title' => t('Status'),
    'help' => t('Whether or not the store was active at the time of the revision.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
      'output formats' => array(
        'active-disabled' => array(t('Active'), t('Disabled')),
      ),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Active'),
      'type' => 'yes-no',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Expose the order revision log.
  $data['commerce_store_revision']['log'] = array(
    'title' => t('Log message'),
    'help' => t('The log message entered when the revision was created.'),
    'field' => array(
      'handler' => 'views_handler_field_xss',
     ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // Expose the revision timestamp.
  $data['commerce_store_revision']['revision_timestamp'] = array(
    'title' => t('Revision date'),
    'help' => t('The date the store revision was created.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  return $data;
}
