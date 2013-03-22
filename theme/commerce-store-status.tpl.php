<?php

/**
 * @file
 * Default theme implementation to present the status on a store page.
 *
 * Available variables:
 * - $status: The string representation of a store's status to render.
 * - $label: If present, the string to use as the status label.
 *
 * Helper variables:
 * - $store: The fully loaded store object the status belongs to.
 */
?>
<?php if ($status): ?>
  <div class="commerce-store-status">
    <?php if ($label): ?>
      <div class="commerce-store-status-label">
        <?php print $label; ?>
      </div>
    <?php endif; ?>
    <?php print $status; ?>
  </div>
<?php endif; ?>
