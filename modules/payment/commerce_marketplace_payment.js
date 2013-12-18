
(function ($) {

Drupal.behaviors.commerceMarketplacePaymentFieldsetSummaries = {
  attach: function (context) {

    // Payment methods fieldset.
    $('fieldset.store-form-payment-methods', context).drupalSetSummary(function (context) {
      var vals = [];

      $('input:checked', context).each(function () {
        vals.push(Drupal.checkPlain($.trim($(this).attr('title'))));
      });

      if (!$('input:checked', context).is(':checked')) {
        vals.unshift(Drupal.t('None'));
      }
      return vals.join(', ');
    });

  }
};

})(jQuery);
