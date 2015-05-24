(function($){
  Drupal.behaviors.field_timer = {
    attach: function() {
      var settings = Drupal.settings.field_timer;
      for (var key in settings) {
				switch (settings[key].plugin) {
					case 'county':
						var options = settings[key].options;
						$('#county-' + key).not('.field-timer-processed').
							county({
								endDateTime: new Date(settings[key].timestamp * 1000),
								animation: options.animation,
								speed: options.speed,
								theme: options.county_theme,
								reflection: options.reflection,
								reflectionOpacity: options.reflectionOpacity,
							}).addClass('field-timer-processed');
						break;

					case 'jquery.countdown':
						var options = settings[key].options;
						$('#jquery-countdown-' + key).not('.field-timer-processed').
							countdown({
								until: options.until ? new Date(settings[key].timestamp * 1000) : null,
								since: options.since ? new Date(settings[key].timestamp * 1000) : null,
								format: options.format,
								layout: options.layout,
								compact: options.compact,
								significant: options.significant,
								timeSeparator: options.timeSeparator,
								padZeroes: options.padZeroes,
							}).addClass('field-timer-processed');
						break;

					case 'jquery.countdown.led':
						var options = settings[key].options;
						$('#jquery-countdown-led-' + key).not('.field-timer-processed').
							countdown({
								until: options.until ? new Date(settings[key].timestamp * 1000) : null,
								since: options.since ? new Date(settings[key].timestamp * 1000) : null,
								layout: $('#jquery-countdown-led-' + key).html(),
							}).addClass('field-timer-processed');
						break;
				}
			}
    }
  }
})(jQuery);
