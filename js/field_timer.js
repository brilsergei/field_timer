/**
 * @file
 * File with JS to initialize jQuery plugins on fields.
 */

(function($){
  Drupal.behaviors.field_timer = {
    attach: function() {
      var settings = drupalSettings.field_timer;
      for (var key in settings) {
        var options = settings[key].settings;
        var $item = $('#' + key);
        var timestamp = $item.data('timestamp');
        switch (settings[key].plugin) {
          case 'county':
            $item.once('field-timer').each(function() {
              $(this).county($.extend({endDateTime: new Date(timestamp * 1000)}, options));
            });
            break;

          case 'jquery.countdown':
            $item.once('field-timer').each(function() {
              $(this).countdown($.extend(
                options,
                {
                  until: (options.until ? new Date(timestamp * 1000) : null),
                  since: (options.since ? new Date(timestamp * 1000) : null)
                }
              ));
            });
            break;

          case 'jquery.countdown.led':
            $item.once('field-timer').each(function() {
              $(this).countdown({
                until: (options.until ? new Date(timestamp * 1000) : null),
                since: (options.since ? new Date(timestamp * 1000) : null),
                layout: $item.html()
              });
            });
            break;
        }
      }
    }
  }
})(jQuery);
