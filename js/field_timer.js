(function($){
  Drupal.behaviors.field_timer = {
    attach: function() {
      for (var type in Drupal.settings.field_timer) {
        for (var id in Drupal.settings.field_timer[type]) {
          for (var delta in Drupal.settings.field_timer[type][id]) {
            $('#jquery-countdown-'+type+'-'+id+'-'+delta).not('.field-timer-processed').
              countdown({
                timestamp: new Date(Drupal.settings.field_timer[type][id][delta] * 1000),
              }).addClass('.field-timer-processed');
          }
        }
      }
    }
  }
})(jQuery);