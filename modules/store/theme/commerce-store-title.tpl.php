<?php

/**
 * @file
 * Default theme implementation to present the title on a store page.
 *
 * Available variables:
 * - $title: The title to render.
 * - $label: If present, the string to use as the title label.
 *
 * Helper variables:
 * - $store: The fully loaded store object the title belongs to.
 */
?>
<?php if ($title): ?>
  <div class="commerce-store-title">
    <?php if ($label): ?>
      <div class="commerce-store-title-label">
        <?php print $label; ?>
      </div>
    <?php endif; ?>
    <?php print $title; ?>
  </div>
<?php endif; ?>
