<?php

/**
 * @file
 * Contains functions to handle all available formatters.
 */

/**
 * Help function. 
 * 
 * Build renderable array for Text formatter.
 */
function _field_timer_text_formatter($items, $settings) {
  $elements = array();

  foreach ($items as $delta => $item) {
    // Check what we should display - timer or countdown.
    switch ($settings['type']) {
      case 'auto':
        $interval = REQUEST_TIME - $item['timestamp'];
        if ($interval > 0) {
          $sign = '+';
        }
        else {
          $sign = '-';
        }
        $time = $sign . format_interval(abs($interval), $settings['granularity']);
        break;

      case 'timer':
        $time = format_interval(REQUEST_TIME - $item['timestamp'], $settings['granularity']);
        break;

      case 'countdown':
        $time = format_interval($item['timestamp'] - REQUEST_TIME, $settings['granularity']);
        break;
    }
    $elements[$delta] = array(
      '#type' => 'markup',
      '#markup' => $time,
    );
  }

  return $elements;
}

/**
 * Help function.
 * 
 * Build renderable array and load neccessary files for County formatter.
 */
function _field_timer_county_formatter($items, $settings, $entity_type, $entity) {
  $info = entity_get_info($entity_type);
  $id_key = $info['entity keys']['id'];
  $js_settings = array();
  $elements = array();

  foreach ($items as $delta => $item) {
    $elements[$delta] = array(
      '#type' => 'markup',
      '#markup' => '<div id="county-' . $entity_type . '-' . $entity->{$id_key} . '-' . $delta . '" ' . 'style="background: ' . $settings['background'] . ';"></div>',
    );
    $js_settings[$entity_type][$entity->{$id_key}][$delta] = $item['timestamp'];
  }
  $js_settings[$entity_type][$entity->{$id_key}]['options'] = $settings;
  $js_settings[$entity_type][$entity->{$id_key}]['plugin'] = 'county';

  // Load library files.
  $library = libraries_load('county');
  $library_loaded = $library && !empty($library['loaded']);
  $js_name = 'county.js';
  $base_path = 'sites/all/libraries/county';
  if (!$library_loaded) {
    drupal_set_message(t("Can't load County library. Please download !url jQuery plugin and extract it to @path, so @js can be found at @full_path.", array(
        '!url' => l(t('County'), 'http://www.egrappler.com/free-jquery-count-down-plugin-county/', array('absolute' => TRUE)),
        '@path' => $base_path,
        '@js' => $js_name,
        '@full_path' => $base_path . '/js/' . $js_name,
    )), 'error');
    return array();
  }

  drupal_add_js(array('field_timer' => $js_settings), 'setting');
  drupal_add_js(drupal_get_path('module', 'field_timer') . '/js/field_timer.js', 'file');

  return $elements;
}

/**
 * Help function.
 * 
 * Build renderable array and load neccessary files for jQuery Countdown 
 * formatter.
 */
function _field_timer_jquery_countdown_formatter($items, $settings, $entity_type, $entity, $instance) {
  $info = entity_get_info($entity_type);
  $id_key = $info['entity keys']['id'];
  $js_settings = array();
  $elements = array();

  foreach ($items as $delta => $item) {
    $elements[$delta] = array(
      '#type' => 'markup',
      '#markup' => '<span id="jquery-countdown-' . $entity_type . '-' . $entity->{$id_key} . '-' . $delta . '" class="jquery-countdown"></span>',
    );
    $js_settings[$entity_type][$entity->{$id_key}][$delta] = $item['timestamp'];
  }

  // Add description, expiry text and url.
  $instance_settings = $instance['settings']['jquery-countdown'];
  $data = array($entity_type => $entity);
  $instance_settings['expiryUrl'] = token_replace($instance_settings['expiryUrl'], $data);
  $instance_settings['expiryText'] = token_replace($instance_settings['expiryText'], $data);
  $instance_settings['description'] = token_replace($instance_settings['description'], $data);

  $js_settings[$entity_type][$entity->{$id_key}]['options'] = array_merge($settings, $instance_settings);
  if ($settings['type'] == 'timer' || ($settings['type'] == 'auto' && $item['timestamp'] < REQUEST_TIME)) {
    $js_settings[$entity_type][$entity->{$id_key}]['options']['until'] = FALSE;
    $js_settings[$entity_type][$entity->{$id_key}]['options']['since'] = TRUE;
  }
  elseif ($settings['type'] == 'countdown' || ($settings['type'] == 'auto' && $item['timestamp'] > REQUEST_TIME)) {
    $js_settings[$entity_type][$entity->{$id_key}]['options']['until'] = TRUE;
    $js_settings[$entity_type][$entity->{$id_key}]['options']['since'] = FALSE;
  }
  $js_settings[$entity_type][$entity->{$id_key}]['plugin'] = 'jquery.countdown';

  // Load library files.
  $library = libraries_load('jquery.countdown');
  $library_loaded = $library && !empty($library['loaded']);
  $js_name = 'jquery.countdown.js';
  $js_name_min = 'jquery.countdown.min.js';
  $base_path = 'sites/all/libraries/jquery.countdown';
  if (!$library_loaded) {
    drupal_set_message(t("Can't load jQuery Countdown library. Please download !url plugin and extract it to @path, so @js and @js_min can be found at @full_path and @full_path_min.", array(
        '!url' => l(t('jQuery Countdown'), 'http://keith-wood.name/countdown.html', array('absolute' => TRUE)),
        '@path' => $base_path,
        '@js' => $js_name,
        '@js_min' => $js_name_min,
        '@full_path' => $base_path . '/' . $js_name,
        '@full_path_min' => $base_path . '/' . $js_name_min,
    )), 'error');
    return array();
  }

  // Add neccessary js and css files.
  drupal_add_js(array('field_timer' => $js_settings), 'setting');
  $path = libraries_get_path('jquery.countdown');
  if (file_exists($path . '/jquery.countdown-' . $settings['regional'] . '.js')) {
    drupal_add_js($path . '/jquery.countdown-' . $settings['regional'] . '.js');
  }
  drupal_add_js(drupal_get_path('module', 'field_timer') . '/js/field_timer.js', 'file');
  drupal_add_css(drupal_get_path('module', 'field_timer') . '/css/field_timer.css');

  return $elements;
}

/**
 * Help function.
 * 
 * Build renderable array and load neccessary files for jQuery Countdown LED
 * formatter.
 */
function _field_timer_jquery_countdown_led_formatter($items, $settings, $entity_type, $entity, $instance) {
  
  $info = entity_get_info($entity_type);
  $id_key = $info['entity keys']['id'];
  $js_settings = array();
  $elements = array();
  
  foreach ($items as $delta => $item) {
    $layout = '';
    if ($settings['display_days']) {
      for ($i = $settings['max_count_of_days']; $i > 0; $i--) {
        $layout .= '<span class="led-image{d1' . substr('00', 0, $i - 1) . '}"></span>';
      }
      $layout .= '<span class="led-imageDay"></span><span class="led-imageSpace"></span>';
    }
    if ($settings['display_hours']) {
      $layout .= '<span class="led-image{h10}"></span><span class="led-image{h1}"></span>';
      if ($settings['display_minutes'] || $settings['display_seconds']) {
        $layout .= '<span class="led-imageSep"></span>';
      }
    }
    if ($settings['display_minutes']) {
      $layout .= '<span class="led-image{m10}"></span><span class="led-image{m1}"></span>';
      if ($settings['display_seconds']) {
        $layout .= '<span class="led-imageSep"></span>';
      }
    }
    if ($settings['display_seconds']) {
      $layout .= '<span class="led-image{s10}"></span><span class="led-image{s1}"></span>';
    }
    $elements[$delta] = array(
      '#type' => 'markup',
      '#markup' => '<span id="jquery-countdown-led-' . $entity_type . '-' . $entity->{$id_key} . '-' . $delta . '" class="jquery-countdown-led">' . $layout . '</span>',
    );
    $js_settings[$entity_type][$entity->{$id_key}][$delta] = $item['timestamp'];
  }
  
  // Add description, expiry text and url.
  $instance_settings = $instance['settings']['jquery-countdown'];
  $data = array($entity_type => $entity);
  $instance_settings['expiryUrl'] = token_replace($instance_settings['expiryUrl'], $data);
  $instance_settings['expiryText'] = token_replace($instance_settings['expiryText'], $data);
  $instance_settings['description'] = token_replace($instance_settings['description'], $data);
  
  $js_settings[$entity_type][$entity->{$id_key}]['options'] = array_merge($settings, $instance_settings);
  if ($settings['type'] == 'timer' || ($settings['type'] == 'auto' && $item['timestamp'] < REQUEST_TIME)) {
    $js_settings[$entity_type][$entity->{$id_key}]['options']['until'] = FALSE;
    $js_settings[$entity_type][$entity->{$id_key}]['options']['since'] = TRUE;
  }
  elseif ($settings['type'] == 'countdown' || ($settings['type'] == 'auto' && $item['timestamp'] > REQUEST_TIME)) {
    $js_settings[$entity_type][$entity->{$id_key}]['options']['until'] = TRUE;
    $js_settings[$entity_type][$entity->{$id_key}]['options']['since'] = FALSE;
  }
  $js_settings[$entity_type][$entity->{$id_key}]['plugin'] = 'jquery.countdown.led';

  // Load library files.
  $library = libraries_load('jquery.countdown');
  $library_loaded = $library && !empty($library['loaded']);
  $js_name = 'jquery.countdown.js';
  $js_name_min = 'jquery.countdown.min.js';
  $base_path = 'sites/all/libraries/jquery.countdown';
  if (!$library_loaded) {
    drupal_set_message(t("Can't load jQuery Countdown library. Please download !url plugin and extract it to @path, so @js and @js_min can be found at @full_path and @full_path_min.", array(
        '!url' => l(t('jQuery Countdown'), 'http://keith-wood.name/countdown.html', array('absolute' => TRUE)),
        '@path' => $base_path,
        '@js' => $js_name,
        '@js_min' => $js_name_min,
        '@full_path' => $base_path . '/' . $js_name,
        '@full_path_min' => $base_path . '/' . $js_name_min,
    )), 'error');
    return array();
  }

  // Add neccessary js and css files.
  drupal_add_js(array('field_timer' => $js_settings), 'setting');
  drupal_add_js(drupal_get_path('module', 'field_timer') . '/js/field_timer.js', 'file');
  drupal_add_css(drupal_get_path('module', 'field_timer') . '/css/field_timer.css');
  // Since we can't get path to image from CSS file, we add this rule here
  drupal_add_css('.jquery-countdown-led span{background: url(/' . libraries_get_path('jquery.countdown') . '/countdownLED.png) no-repeat 0px 0px;}', 'inline');

  return $elements;
}
