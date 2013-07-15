(function($){
  Drupal.behaviors.field_timer = {
    attach: function() {
      var settings = Drupal.settings.field_timer;
      for (var type in settings) {
        for (var id in settings[type]) {
          for (var delta in settings[type][id]) {
            if (settings[type][id].plugin == 'county') {
              var options = settings[type][id].options;
              $('#county-'+type+'-'+id+'-'+delta).not('.field-timer-processed').
                county({
                  endDateTime: new Date(settings[type][id][delta] * 1000),
                  animation: options.animation,
                  speed: options.speed,
                  theme: options.theme,
                  reflection: options.reflection,
                  reflectionOpacity: options.reflectionOpacity,
                }).addClass('.field-timer-processed');
            }
          }
        }
      }
    }
  }
})(jQuery);