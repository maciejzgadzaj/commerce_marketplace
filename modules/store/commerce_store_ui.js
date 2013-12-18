
(function ($) {

Drupal.behaviors.commerceStoreUIFieldsetSummaries = {
  attach: function (context) {

    // Store owner fieldset.
    $('fieldset.store-form-owner', context).drupalSetSummary(function (context) {
      return Drupal.t('@name', { '@name': $('.form-item-owner-name input', context).val() });
    });

    // Revision information fieldset.
    $('fieldset.store-form-revision-information', context).drupalSetSummary(function (context) {
      var revisionCheckbox = $('.form-item-revision input', context);

      // Return 'New revision' if the 'Create new revision' checkbox is checked,
      // or if the checkbox doesn't exist or is not checked, but some text was
      // added to the revision log.
      if (revisionCheckbox.is(':checked') || $('.form-item-revision-information-log textarea', context).val().length) {
        return Drupal.t('New revision');
      }

      return Drupal.t('No revision');
    });

    // Status options fieldset.
    $('fieldset.store-form-options', context).drupalSetSummary(function (context) {
      var vals = [];

      $('input:checked', context).parent().each(function () {
        vals.push(Drupal.checkPlain($.trim($(this).text())));
      });

      if (!$('.form-item-options-status input', context).is(':checked')) {
        vals.unshift(Drupal.t('Disabled'));
      }
      return vals.join(', ');
    });
  }
};

})(jQuery);
