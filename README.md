# Field Timer

## Description

This module provides datetime field formatters to display the field as a
timer or countdown. Module provides 3 field formatters: simple text and
2 formatters based on jQuery plugins: [County](https://github.com/brilsergei/county)
and [jQuery Countdown](http://keith-wood.name/countdown.html).

## Installation

1. Install module using composer or download the module archive.
2. Enable the module using admin module page or drush.

## Translation

Translation of 'Text timer or countdown' formatter can be done on the User
interface translation page.
Translation of jQuery Countdown is not compatible with Drupal translation
system. Therefore a special compatibility layer was created in the module.
Translation of 'jQuery Countdown' formatter can be done by 2 ways:
1. In the formatter settings form check option 'Use system language'. In this
case it will use the first langcode from the list - current content language,
default language, 'en', which will be found in jQuery Countdown translation
sets. For example, if content language is Belarusian (by), default language is
Russian (ru), then countdown will be displayed in Russian because there is no
Belarusian for jQuery Countdown.
2. You may specify Language option for each language enabled in the system
separately. In order to do it uncheck option 'Use system language' in the
formatter settings form, export configs with 'drush cex'. After that you need
find appropriate entity_view_mode entity config file in the folder with
exported configurations, copy the file to the language folder which you want
translate to and change in the copied file option 'regional' for your field.
Formatter settings cannot be translated via interface yet. See
https://www.drupal.org/project/drupal/issues/2546212

## Assets
Required assets of the used JS libs are loaded from [jsDelivr](https://www.jsdelivr.com/).
It is also possible to load the assets from site local storage. In order to use locally stored library assets
download required JS libraries to drupal library directory and rename
library directories to libraries/county and libraries/jquery.countdown
respectively. This module supports jQuery Countdown 2.1.0.

## Issues, Bugs and Feature Requests

Issues, Bugs and Feature Requests should be made on the page at
https://drupal.org/project/issues/2040519.

## Creators

This module was created by [Sergei Brill](http://drupal.org/user/2306590/)
