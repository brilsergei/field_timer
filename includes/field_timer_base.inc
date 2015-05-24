<?php

/**
 * @file
 * Contains help classes to perform field formatter related actions.
 */

/**
 * Feild formatter interface.
 */
interface field_timer_formatter_interface {
  
  /**
   * Check if it is possible to enable current formatter.
   * @return boolean
   */
  public function enableFormatter();

  /**
   * Get formatter definition.
   * @return array Formatter definition.
   */
  public function getFormatterDefinition();
  
  /**
   * Get default formatter settings. All settings for each field type will be
   * combined into one array.
   * @return array Default settings.
   */
  public function getDefaultSettings();
  
  /**
   * Get formatter settings summary.
   * @param array $settings Formatter settings array.
   * @param array $field Field definition.
   * @return string array summary.
   */
  public function getSettingsSummary($settings, $field);
  
  /**
   * Get formatter settings form.
   * @param array $settings Formatter settings array.
   * @param array $field Field definition.
   * @return array Drupal form array to display formetter settings form.
   */
  public function getSettingsForm($settings, $field);
  
  /**
   * Render field items.
   * @param string $entity_type Entity type.
   * @param object $entity Entity object.
   * @param array $field Field definition.
   * @param array $items Field items.
   * @param array $settings Formatter settings.
   * @return array Drupal renderable array.
   */
  public function renderItems($entity_type, $entity, $field, $instance, $langcode, $items, $display);
  
  /**
   * Get formatter name.
   */
  public function getFormatterName();
  
  /**
   * Get field type.
   */
  public function getFieldType();
}

/**
 * Base class.
 */
abstract class field_timer_formatter_base implements field_timer_formatter_interface {
  
  /**
   * Convert field item value to timstamp.
   * @param array $item Field item.
   * @param array $field Field definition.
   * @param mixed $key Array key to get timestamp from.
   * @return int Timestamp.
   */
  abstract protected function getTimestamp($item, $field, $key = '');
}

/**
 * Base class for formatters using js library.
 */
abstract class field_timer_formatter_js_base extends field_timer_formatter_base implements field_timer_formatter_interface {
  
  protected $library_loaded = FALSE;
  
  /**
   * Get jQuery plugin name.
   */
  abstract public function getPluginName();

  /**
   * Load library files.
   * @return boolean Indicate if library files loaded successfully.
   */
  abstract protected function loadLibrary();
  
  /**
   * Load custom JS files and JS settings.
   */
  protected function loadJSandCSS($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    $js_settings = $this->generateJSSettings($entity_type, $entity, $field, $instance, $langcode, $items, $display);

    drupal_add_js(array('field_timer' => $js_settings), 'setting');
    drupal_add_js(drupal_get_path('module', 'field_timer') . '/js/field_timer.js', 'file');
  }
  
  /**
   * Generate JS settings.
   */
  abstract protected function generateJSSettings($entity_type, $entity, $field, $instance, $langcode, $items, $display);

}

/**
 * Base class for jquery_countdown formatters.
 */
abstract class field_timer_jquery_countdown_js_base extends field_timer_formatter_js_base {
  /**
   * @inheritdoc
   */
  public function enableFormatter() {
    $library = libraries_detect('jquery.countdown');
    return $library && !empty($library['installed']);
  }
  
  /**
   * @inheritdoc
   */
  protected function loadLibrary() {
    if (!$this->library_loaded) {
      $library = libraries_load('jquery.countdown');
      $this->library_loaded = $library && !empty($library['loaded']);
      $js_name = 'jquery.countdown.js';
      $js_name_min = 'jquery.countdown.min.js';
      $base_path = 'sites/all/libraries/jquery.countdown';
      if (!$this->library_loaded) {
        drupal_set_message(t("Can't load jQuery Countdown library. Please download !url plugin and extract it to @path, so @js and @js_min can be found at @full_path and @full_path_min.", array(
          '!url' => l(t('jQuery Countdown'), 'http://keith-wood.name/countdown.html', array('absolute' => TRUE)),
          '@path' => $base_path,
          '@js' => $js_name,
          '@js_min' => $js_name_min,
          '@full_path' => $base_path . '/' . $js_name,
          '@full_path_min' => $base_path . '/' . $js_name_min,
      )), 'error');
      }
    }
    
    return $this->library_loaded;
  }
  
  /**
   * @inheritdoc
   */
  protected function generateJSSettings($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    $info = entity_get_info($entity_type);
    $id_key = $info['entity keys']['id'];
    $js_settings = array();
    $settings = $display['settings'];
    
    foreach ($items as $delta => $item) {
      $key = $entity_type . '_' . $entity->{$id_key} . '_' . $field['field_name'] . '_' . $delta;
      $timestamp = $this->getTimestamp($item, $field);
      $js_settings[$key]['timestamp'] = $timestamp;
      $js_settings[$key]['options'] = $settings;
      if ($settings['type'] == 'timer' || ($settings['type'] == 'auto' && $timestamp <= REQUEST_TIME)) {
        $js_settings[$key]['options']['until'] = FALSE;
        $js_settings[$key]['options']['since'] = TRUE;
      }
      elseif ($settings['type'] == 'countdown' || ($settings['type'] == 'auto' && $timestamp > REQUEST_TIME)) {
        $js_settings[$key]['options']['until'] = TRUE;
        $js_settings[$key]['options']['since'] = FALSE;
      }
      $js_settings[$key]['plugin'] = $this->getPluginName();
    }
    
    return $js_settings;
  }
  
  /**
   * @inheritdoc
   */
  protected function loadJSandCSS($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    parent::loadJSandCSS($entity_type, $entity, $field, $instance, $langcode, $items, $display);
    drupal_add_css(drupal_get_path('module', 'field_timer') . '/css/field_timer.css', 'file');
  }

  protected function formatterTypes() {
    return array(
      'auto' => t('Auto'),
      'timer' => t('Timer'),
      'countdown' => t('Countdown'),
    );
  }
}
