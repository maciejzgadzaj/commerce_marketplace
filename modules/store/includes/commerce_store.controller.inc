<?php

/**
 * @file
 * The controller for the store entity containing the CRUD operations.
 */

/**
 * The controller class for stores contains methods for the store CRUD operations.
 *
 * Mainly relies on the EntityAPIController class provided by the Entity
 * module, just overrides specific features.
 */
class CommerceStoreEntityController extends DrupalCommerceEntityController {

  /**
   * Unserializes the data property of loaded stores.
   */
  public function attachLoad(&$queried_stores, $revision_id = FALSE) {
    foreach ($queried_stores as $store_id => &$store) {
      $store->data = unserialize($store->data);
    }

    // Call the default attachLoad() method. This will add fields.
    parent::attachLoad($queried_stores, $revision_id);
  }

  /**
   * Create a default store.
   *
   * @param array $values
   *   An array of values to set, keyed by property name.
   * @return
   *   A store object with all default fields initialized.
   */
  public function create(array $values = array()) {
    $values += array(
      'store_id' => NULL,
      'is_new' => TRUE,
      'revision_id' => NULL,
      'title' => '',
      'uid' => '',
      'status' => 1,
      'created' => '',
      'changed' => '',
    );

    return parent::create($values);
  }

  /**
   * Saves a store.
   *
   * @param $store
   *   The full store object to save.
   * @param $transaction
   *   An optional transaction object.
   *
   * @return
   *   SAVED_NEW or SAVED_UPDATED depending on the operation performed.
   */
  public function save($store, DatabaseTransaction $transaction = NULL) {
    global $user;

    // Hardcode the changed time.
    $store->changed = REQUEST_TIME;

    if (empty($store->{$this->idKey}) || !empty($store->is_new)) {
      // Set the creation timestamp if not set, for new entities.
      if (empty($store->created)) {
        $store->created = REQUEST_TIME;
      }
    }

    $store->revision_timestamp = REQUEST_TIME;
    $store->revision_uid = $user->uid;

    // Determine if we will be inserting a new store.
    $store->is_new = empty($store->store_id);

    if ($store->is_new || !empty($store->revision)) {
      // When inserting either a new store or revision, $entity->log must be set
      // because {commerce_store_revision}.log is a text column and therefore
      // cannot have a default value. However, it might not be set at this
      // point, so we ensure that it is at least an empty string in that case.
      if (!isset($store->log)) {
        $store->log = '';
      }
    }
    elseif (empty($store->log)) {
      // If we are updating an existing store without adding a new revision,
      // we need to make sure $entity->log is unset whenever it is empty. As
      // long as $entity->log is unset, drupal_write_record() will not attempt
      // to update the existing database column when re-saving the revision.
      unset($store->log);
    }

    // Remove price components from any price fields attached to the store.
    // Default price components should instead be rebuilt each load using
    // hook_field_attach_load().
    foreach (field_info_instances('commerce_store', $store->type) as $field_name => $instance) {
      // Load the instance's field data.
      $field = field_info_field($instance['field_name']);

      // If the instance is a price field with data on this store...
      if ($field['type'] == 'commerce_price' && !empty($store->{$field_name})) {
        // Remove the price components from every price value.
        foreach ($store->{$field_name} as $langcode => &$items) {
          foreach ($items as $delta => &$item) {
            if (!empty($item['data'])) {
              $item['data']['components'] = array();
            }
          }
        }
      }
    }

    return parent::save($store, $transaction);
  }

  /**
   * Deletes multiple stores by ID.
   *
   * @param $store_ids
   *   An array of store IDs to delete.
   * @param $transaction
   *   An optional transaction object.
   *
   * @return
   *   TRUE on success, FALSE otherwise.
   */
  public function delete($store_ids, DatabaseTransaction $transaction = NULL) {
    if (!empty($store_ids)) {
      $stores = $this->load($store_ids, array());

      // Ensure the stores can actually be deleted.
      foreach ((array) $stores as $store_id => $store) {
        if (!commerce_store_can_delete($store)) {
          unset($stores[$store_id]);
        }
      }

      // If none of the specified stores can be deleted, return FALSE.
      if (empty($stores)) {
        return FALSE;
      }

      parent::delete(array_keys($stores), $transaction);
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Builds a structured array representing the entity's content.
   *
   * The content built for the entity will vary depending on the $view_mode
   * parameter.
   *
   * @param $entity
   *   An entity object.
   * @param $view_mode
   *   View mode, e.g. 'full', 'teaser'...
   * @param $langcode
   *   (optional) A language code to use for rendering. Defaults to the global
   *   content language of the current request.
   * @return
   *   The renderable array.
   */
  public function buildContent($store, $view_mode = 'full', $langcode = NULL, $content = array()) {
    // Prepare a reusable array representing the CSS file to attach to the view.
    $attached = array(
      'css' => array(drupal_get_path('module', 'commerce_store') . '/theme/commerce_store.theme.css'),
    );

    // Add the default fields inherent to the store entity.
    $content['title'] = array(
      '#markup' => theme('commerce_store_title', array('title' => $store->title, 'label' => t('Title:'), 'store' => $store)),
      '#attached' => $attached,
    );
    $content['status'] = array(
      '#markup' => theme('commerce_store_status', array('status' => $store->status, 'label' => t('Status:'), 'store' => $store)),
      '#attached' => $attached,
    );

    return parent::buildContent($store, $view_mode, $langcode, $content);
  }
}
