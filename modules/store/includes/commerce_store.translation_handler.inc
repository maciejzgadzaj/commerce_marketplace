<?php

/**
 * @file
 * Translation handler for the Commerce Store entity.
 */

/**
 * Commerce Store translation handler.
 *
 * This class is registered in commerce_store.module using the translation
 * property the Entity Translation module uses via hook_entity_info().
 *
 * @see commerce_store_entity_info()
 */
class EntityTranslationCommerceStoreHandler extends EntityTranslationDefaultHandler {

  public function __construct($entity_type, $entity_info, $entity) {
    parent::__construct('commerce_store', $entity_info, $entity);
  }

  /**
   * Indicates whether this commerce store is a revision or not.
   */
  public function isRevision() {
    return !empty($this->store->revision);
  }

  /**
   * Checks whether the current user has access to this store.
   */
  public function getAccess($op) {
    return commerce_store_access($op, $this->entity);
  }

  /**
   * Tweaks the store form to support multilingual elements.
   */
  public function entityForm(&$form, &$form_state) {
    parent::entityForm($form, $form_state);
    if (isset($form['change_history']['#weight'])) {
      $form['translation']['#weight'] = $form['change_history']['#weight'] - 0.01;
    }
    $form['actions']['delete_translation']['#suffix'] = $form['actions']['submit']['#suffix'];
    unset($form['actions']['submit']['#suffix']);
  }

  /**
   * Returns whether the store is active (TRUE) or disabled (FALSE).
   */
  protected function getStatus() {
    return (boolean) $this->entity->status;
  }
}
