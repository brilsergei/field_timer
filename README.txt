h1. Field Timer

h2. Description

This module provides datatime field formatters to display the field as a
timer/countdown. Module provides 3 field formatters: simple text and
2 formatters based on jQuery plugins: County and jQuery Countdown.

h2. Requirements

County (http://www.egrappler.com/free-jquery-count-down-plugin-county/)
jQuery Countdown (http://keith-wood.name/countdown.html)

h2. Installation

1. Download the module.
2. Download required JS libraries to drupal library directory and rename
library directories to libraries/county and libraries/jquery.countdown
respectively. This module supports jQuery Countdown 2.0.2.
3. Remove first css rule from sites/all/libraries/county/css/county.css. It
changes font size on the site. Plugin author was notified about this problem.
I hope he'll fix it.
4. Enable the module using admin module page or drush.

h2. Issues, Bugs and Feature Requests

Issues, Bugs and Feature Requests should be made on the page at 
https://drupal.org/project/issues/2040519.

h2. Creators

This module was created by Sergei Brill 
("Drupal user sergei_brill": http://drupal.org/user/2306590/)
