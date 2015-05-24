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
 * Base class for field_timer_text formatter.
 */
abstract class field_timer_text_base extends field_timer_formatter_base implements field_timer_formatter_interface {
  
  /**
   * @inheritdoc
   */
  public function enableFormatter() {
    return TRUE;
  }

  /**
   * @inheritdoc
   */
  final public function getFormatterDefinition() {
    return array (
      'label' => t('Text'),
      'description' => t('Simple text timer and countdown.'),
    );
  }


  /**
   * @inheritdoc
   */
  public function getDefaultSettings() {
    return array(
      'type' => 'auto',
      'granularity' => 2,
    );
  }
  
  /**
   * @inheritdoc
   */
  public function getSettingsSummary($settings, $field) {
    $summary = array();
    $types = $this->formatterTypes();
    $summary[] = t('Type: %type', array('%type' => $types[$settings['type']]));
    $summary[] = t('Granularity: %granularity', array('%granularity' => $settings['granularity']));

    return $summary;
  }
  
  /**
   * @inheritdoc
   */
  public function getSettingsForm($settings, $field) {
    $settings_form = array();
    $settings_form['type'] = array(
      '#type' => 'select',
      '#title' => t('Type'),
      '#options' => $this->formatterTypes(),
      '#default_value' => $settings['type'],
    );

    $settings_form['granularity'] = array(
      '#type' => 'select',
      '#title' => t('Granularity'),
      '#options' => range(0, 7),
      '#default_value' => $settings['granularity'],
    );

    return $settings_form;
  }
  
  public function renderItems($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    $elements = array();
    $settings = $display['settings'];

    foreach ($items as $delta => $item) {
      // Check what we should display - timer or countdown.
      switch ($settings['type']) {
        case 'auto':
          $interval = REQUEST_TIME - $this->getTimestamp($item, $field);
          if ($interval > 0) {
            $sign = '+';
          }
          else {
            $sign = '-';
          }
          $time = $sign . format_interval(abs($interval), $settings['granularity']);
          break;

        case 'timer':
          $time = format_interval(REQUEST_TIME - $this->getTimestamp($item, $field), $settings['granularity']);
          break;

        case 'countdown':
          $time = format_interval($this->getTimestamp($item, $field) - REQUEST_TIME, $settings['granularity']);
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
   * @inheritdoc
   */
  public function getFormatterName() {
    return 'field_timer_text';
  }
  
  protected function formatterTypes() {
    return array(
      'auto' => t('Auto'),
      'timer' => t('Timer'),
      'countdown' => t('Countdown'),
    );
  }
}

/**
 * Class for field_timer_text formatter and field_timer field type.
 */
class field_timer_text_field_timer extends field_timer_text_base {

  /**
   * @inheritdoc
   */
  public function getFieldType() {
    return 'field_timer';
  }

  /**
   * @inheritdoc
   */
  protected function getTimestamp($item, $field, $key = 'timestamp') {
    return $item[$key];
  }
}

/**
 * Base class for field_timer_text formatter and date field types.
 */
abstract class field_timer_text_date_field_base extends field_timer_text_base {

  /**
   * @inheritdoc
   */
  public function getDefaultSettings() {
    return parent::getDefaultSettings() + array(
      'title' => '',
      'title2' => '',
      'type2' => 'auto',
      'granularity2' => 2,
      'fromto' => 'both',
    );
  }
  
  /**
   * @inheritdoc
   */
  public function getSettingsSummary($settings, $field) {
    $summary = array();
    if (!empty($field['settings']['todate'])) {
      $types = $this->formatterTypes();
      $displays = $this->formatterDisplays();
      $summary[] = t('Type for Start date: %type', array('%type' => $types[$settings['type']]));
      $summary[] = t('Granularity for Start date: %granularity', array('%granularity' => $settings['granularity']));
      $summary[] = t('Title for Start date: %title', array('%title' => $settings['title']));
      $summary[] = t('Type for End date: %type', array('%type' => $types[$settings['type2']]));
      $summary[] = t('Granularity for End date: %granularity', array('%granularity' => $settings['granularity2']));
      $summary[] = t('Title for End date: %title', array('%title' => $settings['title2']));
      $summary[] = t('Display: %display', array('%display' => $displays[$settings['fromto']]));
    }
    else {
      $summary[] = t('Title: %title', array('%title' => $settings['title']));
      $summary = array_merge($summary, parent::getSettingsSummary($settings, $field));
    }

    return $summary;
  }
  
  /**
   * @inheritdoc
   */
  public function getSettingsForm($settings, $field) {
    $settings_form = parent::getSettingsForm($settings, $field);
    
    $settings_form['title'] = array(
      '#type' => 'textfield',
      '#title' => empty($field['settings']['todate']) ? t('Title') : t('Title for Start date'),
      '#default_value' => $settings['title'],
      '#weight' => -10,
    );
    
    if (!empty($field['settings']['todate'])) {
      $settings_form['type']['#title'] = t('Type for Start date');
      $settings_form['granularity']['#title'] = t('Granularity for Start date');   
      $settings_form['title2'] = array(
        '#type' => 'textfield',
        '#title' => t('Title for End date'),
        '#default_value' => $settings['title2'],
      );

      $settings_form['type2'] = array(
        '#type' => 'select',
        '#title' => t('Type for End date'),
        '#options' => $this->formatterTypes(),
        '#default_value' => $settings['type2'],
      );

      $settings_form['granularity2'] = array(
        '#type' => 'select',
        '#title' => t('Granularity for End date'),
        '#options' => range(0, 7),
        '#default_value' => $settings['granularity2'],
      );

      $settings_form['fromto'] = array(
        '#type' => 'select',
        '#title' => t('Display'),
        '#options' => $this->formatterDisplays(),
        '#default_value' => $settings['fromto'],
      );
    }
    
    return $settings_form;
  }

  /**
   * @inheritdoc
   */
  public function renderItems($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    $elements = ($settings['fromto'] != 'to') ? parent::renderItems($entity_type, $entity, $field, $items, $settings) : array();
    $settings = $display['settings'];

    foreach ($items as $delta => $item) {
      if (!empty($settings['title']) && $settings['fromto'] != 'to') {
        $elements[$delta]['#markup'] = '<span class="field-timer-start-date-title">'
        . $settings['title'] . '</span> ' . $elements[$delta]['#markup'];
      }
      if ($settings['fromto'] != 'from' && !empty($field['settings']['todate'])) {
        switch ($settings['type2']) {
          case 'auto':
            $interval = REQUEST_TIME - $this->getTimestamp($item, $field, 'value2');
            if ($interval > 0) {
              $sign = '+';
            }
            else {
              $sign = '-';
            }
            $time = $sign . format_interval(abs($interval), $settings['granularity2']);
            break;

          case 'timer':
            $time = format_interval(REQUEST_TIME - $this->getTimestamp($item, $field, 'value2'), $settings['granularity2']);
            break;

          case 'countdown':
            $time = format_interval($this->getTimestamp($item, $field, 'value2') - REQUEST_TIME, $settings['granularity2']);
            break;
        }
        $title = (!empty($settings['title2'])) ? '<span class="field-timer-end-date-title">' . $settings['title2'] . '</span> ' : '';
        if ($settings['fromto'] == 'to') {
          $elements[$delta] = array(
            '#type' => 'markup',
            '#markup' => $title . $time,
          );
        }
        else {
          $elements[$delta]['#markup'] .= ' ' . $title . $time;
        }
      }
    }
    
    return $elements;
  }

  protected function formatterDisplays() {
    return array(
      'both' => t('Both Start and End dates'),
      'from' => t('Start date only'),
      'to' => t('End date only'),
    );
  }
}

/**
 * Class for field_timer_text formatter and datestamp field type.
 */
class field_timer_text_datestamp extends field_timer_text_date_field_base {
  
  /**
   * @inheritdoc
   */
  public function getFieldType() {
    return 'datestamp';
  }
  /**
   * @inheritdoc
   */
  protected function getTimestamp($item, $field, $key = 'value') {
    return $item[$key];
  }
}

/**
 * Class for field_timer_text formatter and date field type.
 */
class field_timer_text_date extends field_timer_text_date_field_base {
  
  /**
   * @inheritdoc
   */
  public function getFieldType() {
    return 'date';
  }

  /**
   * @inheritdoc
   */
  protected function getTimestamp($item, $field, $key = 'value') {
    $timezone_db = date_get_timezone_db($field['settings']['tz_handling']);
    $db_format = date_type_format($field['type']);
    $date = new DateObject($item[$key], $timezone_db, $db_format);
    return $date->getTimestamp();
  }
}

/**
 * Class for field_timer_text formatter and datetime field type.
 */
class field_timer_text_datetime extends field_timer_text_date_field_base {
  
  /**
   * @inheritdoc
   */
  public function getFieldType() {
    return 'datetime';
  }

  /**
   * @inheritdoc
   */
  protected function getTimestamp($item, $field, $key = 'value') {
    $timezone_db = date_get_timezone_db($field['settings']['tz_handling']);
    $db_format = date_type_format($field['type']);
    $date = new DateObject($item[$key], $timezone_db, $db_format);
    return $date->getTimestamp();
  }
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
 * Base class for field_timer_county formatter.
 */
abstract class field_timer_county_base extends field_timer_formatter_js_base {

  /**
   * @inheritdoc
   */
  public function enableFormatter() {
    $library = libraries_detect('county');
    return $library && !empty($library['installed']);
  }

  /**
   * @inheritdoc
   */
  final public function getFormatterDefinition() {
    return array (
      'label' => t('County'),
      'description' => t('Use County jQuery plugin to display countdown.'),
    );
  }


  /**
   * @inheritdoc
   */
  public function getDefaultSettings() {
    return array(
      'animation' => 'fade',
      'speed' => 500,
      'county_theme' => 'blue',
      'background' => '',
      'reflection' => 1,
      'reflectionOpacity' => 0.2,
    );
  }
  
  /**
   * @inheritdoc
   */
  public function getSettingsSummary($settings, $field) {
    $summary = array();
    $animations = $this->formatterAnimations();
    $themes = $this->formatterThemes();
    $summary[] = t('Animation: %animation', array('%animation' => $animations[$settings['animation']]));
    $summary[] = t('Speed: %speed', array('%speed' => $settings['speed'] . 'ms'));
    $summary[] = t('Theme: %theme', array('%theme' => $themes[$settings['county_theme']]));
    $summary[] = t('Background: %css', array('%css' => $settings['background']));
    $summary[] = t('Reflection: %state', array(
      '%state' => $settings['reflection'] ? 'Enabled' : 'Disabled',
    ));
    if ($settings['reflection']) {
      $summary[] = t('Reflection opacity: %opacity', array(
          '%opacity' => $settings['reflectionOpacity'],
      ));
    }

    return $summary;
  }
  
  /**
   * @inheritdoc
   */
  public function getSettingsForm($settings, $field) {
    $settings_form = array();
    $settings_form['animation'] = array(
      '#type' => 'select',
      '#title' => t('Animation'),
      '#options' => $this->formatterAnimations(),
      '#default_value' => $settings['animation'],
    );

    $settings_form['speed'] = array(
      '#type' => 'textfield',
      '#title' => t('Speed'),
      '#default_value' => $settings['speed'],
    );

    $settings_form['county_theme'] = array(
      '#type' => 'select',
      '#title' => t('Theme'),
      '#options' => $this->formatterThemes(),
      '#default_value' => $settings['county_theme'],
    );

    $settings_form['background'] = array(
      '#type' => 'textfield',
      '#title' => t('Background'),
      '#default_value' => $settings['background'],
      '#description' => t("Data from this field will be added to css property 'background'."),
    );

    $settings_form['reflection'] = array(
      '#type' => 'checkbox',
      '#title' => t('Add reflection'),
      '#default_value' => $settings['reflection'],
    );

    $settings_form['reflectionOpacity'] = array(
      '#type' => 'textfield',
      '#title' => t('Reflection opacity'),
      '#default_value' => $settings['reflectionOpacity'],
      '#description' => t('Float value between 0 and 1.'),
    );

    return $settings_form;
  }
  
  /**
   * @inheritdoc
   */
  public function renderItems($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    if (!$this->loadLibrary()) {
      return array();
    }

    $info = entity_get_info($entity_type);
    $id_key = $info['entity keys']['id'];
    $elements = array();
    $settings = $display['settings'];

    foreach ($items as $delta => $item) {
      $elements[$delta] = array(
        '#type' => 'markup',
        '#markup' => '<div id="county-' . $entity_type . '_' . $entity->{$id_key}
        . '_' . $field['field_name'] . '_' . $delta . '" ' . 'style="background: ' . $settings['background'] . ';"></div>',
      );
    }

    $this->loadJSandCSS($entity_type, $entity, $field, $instance, $langcode, $items, $display);

    return $elements;
  }
  
  /**
   * @inheritdoc
   */
  public function getFormatterName() {
    return 'field_timer_county';
  }
  
  /**
   * @inheritdoc
   */
  public function getPluginName() {
    return 'county';
  }
  
  /**
   * @inheritdoc
   */
  protected function loadLibrary() {
    if (!$this->library_loaded) {
      $library = libraries_load('county');
      $this->library_loaded = $library && !empty($library['loaded']);
      $js_name = 'county.js';
      $base_path = 'sites/all/libraries/county';
      if (!$this->library_loaded) {
        drupal_set_message(t("Can't load County library. Please download !url jQuery plugin and extract it to @path, so @js can be found at @full_path.", array(
            '!url' => l(t('County'), 'http://www.egrappler.com/free-jquery-count-down-plugin-county/', array('absolute' => TRUE)),
            '@path' => $base_path,
            '@js' => $js_name,
            '@full_path' => $base_path . '/js/' . $js_name,
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
      $js_settings[$key]['timestamp'] = $this->getTimestamp($item, $field);
      $js_settings[$key]['options'] = $settings;
      $js_settings[$key]['plugin'] = $this->getPluginName();
    }
    
    return $js_settings;
  }

  protected function formatterAnimations() {
    return array(
      'fade' => t('Fade'),
      'scroll' => t('Scroll'),
    );
  }
  
  protected function formatterThemes() {
    return array(
      'black' => t('Black'),
      'gray' => t('Gray'),
      'red' => t('Red'),
      'blue' => t('Blue'),
    );
  }
}

/**
 * Class for field_timer_county_formatter and field_timer field type.
 */
class field_timer_county_field_timer extends field_timer_county_base {
  
  /**
   * @inheritdoc
   */
  public function getFieldType() {
    return 'field_timer';
  }

  /**
   * @inheritdoc
   */
  protected function getTimestamp($item, $field, $key = 'timestamp') {
    return $item[$key];
  }
}

/**
 * Base class for field_timer_text formatter and date field types.
 */
abstract class field_timer_county_date_field_base extends field_timer_county_base {
  
  /**
   * @inheritdoc
   */
  public function getDefaultSettings() {
    return parent::getDefaultSettings() + array(
      'title' => '',
      'title2' => '',
      'animation2' => 'fade',
      'speed2' => 500,
      'county_theme2' => 'blue',
      'background2' => '',
      'reflection2' => 1,
      'reflectionOpacity2' => 0.2,
      'fromto' => 'both',
    );
  }
  
  /**
   * @inheritdoc
   */
  public function getSettingsSummary($settings, $field) {
    $summary = array();
    if (!empty($field['settings']['todate'])) {
      $animations = $this->formatterAnimations();
      $themes = $this->formatterThemes();
      $displays = $this->formatterDisplays();
      $summary[] = t('Title for Start date: %title', array('%title' => $settings['title']));
      $summary[] = t('Animation for Start date: %animation', array('%animation' => $animations[$settings['animation']]));
      $summary[] = t('Speed for  Start date: %speed', array('%speed' => $settings['speed'] . 'ms'));
      $summary[] = t('Theme for Start date: %theme', array('%theme' => $themes[$settings['county_theme']]));
      $summary[] = t('Background for Start date: %css', array('%css' => $settings['background']));
      $summary[] = t('Reflection for Start date: %state', array(
        '%state' => $settings['reflection'] ? 'Enabled' : 'Disabled',
      ));
      if ($settings['reflection']) {
        $summary[] = t('Reflection opacity for Start date: %opacity', array(
            '%opacity' => $settings['reflectionOpacity'],
        ));
      }
      $summary[] = t('Title for End date: %title', array('%title' => $settings['title2']));
      $summary[] = t('Animation for End date: %animation', array('%animation' => $animations[$settings['animation2']]));
      $summary[] = t('Speed for  End date: %speed', array('%speed' => $settings['speed2'] . 'ms'));
      $summary[] = t('Theme for End date: %theme', array('%theme' => $themes[$settings['county_theme2']]));
      $summary[] = t('Background for End date: %css', array('%css' => $settings['background2']));
      $summary[] = t('Reflection for End date: %state', array(
        '%state' => $settings['reflection2'] ? 'Enabled' : 'Disabled',
      ));
      if ($settings['reflection2']) {
        $summary[] = t('Reflection opacity for End date: %opacity', array(
            '%opacity' => $settings['reflectionOpacity2'],
        ));
      }
      $summary[] = t('Display: %display', array('%display' => $displays[$settings['fromto']]));
    }
    else {
      $summary[] = t('Title: %title', array('%title' => $settings['title']));
      $summary = array_merge($summary, parent::getSettingsSummary($settings, $field));
    }

    return $summary;
  }
  
  /**
   * @inheritdoc
   */
  public function getSettingsForm($settings, $field) {
    $settings_form = parent::getSettingsForm($settings, $field);
    
    $settings_form['title'] = array(
      '#type' => 'textfield',
      '#title' => empty($field['settings']['todate']) ? t('Title') : t('Title for Start date'),
      '#default_value' => $settings['title'],
      '#weight' => -10,
    );
    
    if (!empty($field['settings']['todate'])) {
      $settings_form['animation']['#title'] = t('Animation for Start date');
      $settings_form['speed']['#title'] = t('Speed for Start date');
      $settings_form['county_theme']['#title'] = t('Theme for Start date');
      $settings_form['background']['#title'] = t('Background for Start date');
      $settings_form['reflection']['#title'] = t('Add reflection for Start date');
      $settings_form['reflectionOpacity']['#title'] = t('Reflection opacity for Start date');
      
      $settings_form['title2'] = array(
      '#type' => 'textfield',
      '#title' => t('Title for End date'),
      '#default_value' => $settings['title2'],
    );
      
      $settings_form['animation2'] = array(
        '#type' => 'select',
        '#title' => t('Animation for End date'),
        '#options' => $this->formatterAnimations(),
        '#default_value' => $settings['animation2'],
      );

      $settings_form['speed2'] = array(
        '#type' => 'textfield',
        '#title' => t('Speed for End date'),
        '#default_value' => $settings['speed2'],
      );

      $settings_form['county_theme2'] = array(
        '#type' => 'select',
        '#title' => t('Theme for End date'),
        '#options' => $this->formatterThemes(),
        '#default_value' => $settings['county_theme2'],
      );

      $settings_form['background2'] = array(
        '#type' => 'textfield',
        '#title' => t('Background for End date'),
        '#default_value' => $settings['background2'],
        '#description' => t("Data from this field will be added to css property 'background'."),
      );

      $settings_form['reflection2'] = array(
        '#type' => 'checkbox',
        '#title' => t('Add reflection for End date'),
        '#default_value' => $settings['reflection2'],
      );

      $settings_form['reflectionOpacity2'] = array(
        '#type' => 'textfield',
        '#title' => t('Reflection opacity for End date'),
        '#default_value' => $settings['reflectionOpacity2'],
        '#description' => t('Float value between 0 and 1.'),
      );
      
      $settings_form['fromto'] = array(
        '#type' => 'select',
        '#title' => t('Display'),
        '#options' => $this->formatterDisplays(),
        '#default_value' => $settings['fromto'],
      );
    }
    
    return $settings_form;
  }

  /**
   * @inheritdoc
   */
  public function renderItems($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    $settings = $display['settings'];
    $elements = ($settings['fromto'] != 'to') ? parent::renderItems($entity_type, $entity, $field, $instance, $langcode, $items, $display) : array();
    $info = entity_get_info($entity_type);
    $id_key = $info['entity keys']['id'];

    foreach ($items as $delta => $item) {
      if (!empty($settings['title']) && $settings['fromto'] != 'to') {
        $elements[$delta]['#markup'] = '<span class="field-timer-start-date-title">'
        . $settings['title'] . '</span> ' . $elements[$delta]['#markup'];
      }
      if ($settings['fromto'] != 'from' && !empty($field['settings']['todate'])) {
        $title = (!empty($settings['title2'])) ? '<span class="field-timer-end-date-title">' . $settings['title2'] . '</span> ' : '';
        $time = '<div id="county-' . $entity_type . '_' . $entity->{$id_key}
        . '_' . $field['field_name'] . '_' . $delta . '_2" ' . 'style="background: ' . $settings['background2'] . ';"></div>';
        if ($settings['fromto'] == 'to') {
          $elements[$delta] = array(
            '#type' => 'markup',
            '#markup' => $title . $time,
          );
        }
        else {
          $elements[$delta]['#markup'] .= ' ' . $title . $time;
        }
      }
    }

    return $elements;
  }
  
  /**
   * @inheritdoc
   */
  protected function generateJSSettings($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    $js_settings = parent::generateJSSettings($entity_type, $entity, $field, $instance, $langcode, $items, $display);
    if (!empty($field['settings']['todate'])) {
      $info = entity_get_info($entity_type);
      $id_key = $info['entity keys']['id'];
      $settings = $display['settings'];

      $settings2 = array();
      foreach ($settings as $name => $value) {
        if (substr($name, strlen($name - 2)) == '2') {
          $settings2[substr($name, 0, strlen($name) - 1)] = $value;
        }
      }
      
      foreach ($items as $delta => $item) {
        $key = $entity_type . '_' . $entity->{$id_key} . '_' . $field['field_name'] . '_' . $delta . '_2';
        $js_settings[$key]['timestamp'] = $this->getTimestamp($item, $field, 'value2');
        $js_settings[$key]['options'] = $settings2;
        $js_settings[$key]['plugin'] = $this->getPluginName();
      }
    }
    
    return $js_settings;
  }

  protected function formatterDisplays() {
    return array(
      'both' => t('Both Start and End dates'),
      'from' => t('Start date only'),
      'to' => t('End date only'),
    );
  }
}

/**
 * Class for field_timer_text formatter and datestamp field type.
 */
class field_timer_county_datestamp extends field_timer_county_date_field_base {
  
  /**
   * @inheritdoc
   */
  public function getFieldType() {
    return 'datestamp';
  }

  /**
   * @inheritdoc
   */
  protected function getTimestamp($item, $field, $key = 'value') {
    return $item[$key];
  }
}

/**
 * Class for field_timer_text formatter and date field type.
 */
class field_timer_county_date extends field_timer_county_date_field_base {
  
  /**
   * @inheritdoc
   */
  public function getFieldType() {
    return 'date';
  }

  /**
   * @inheritdoc
   */
  protected function getTimestamp($item, $field, $key = 'value') {
    $timezone_db = date_get_timezone_db($field['settings']['tz_handling']);
    $db_format = date_type_format($field['type']);
    $date = new DateObject($item[$key], $timezone_db, $db_format);
    return $date->getTimestamp();
  }
}

/**
 * Class for field_timer_text formatter and datetime field type.
 */
class field_timer_county_datetime extends field_timer_county_date_field_base {
  
  /**
   * @inheritdoc
   */
  public function getFieldType() {
    return 'datetime';
  }

  /**
   * @inheritdoc
   */
  protected function getTimestamp($item, $field, $key = 'value') {
    $timezone_db = date_get_timezone_db($field['settings']['tz_handling']);
    $db_format = date_type_format($field['type']);
    $date = new DateObject($item[$key], $timezone_db, $db_format);
    return $date->getTimestamp();
  }
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

/**
 * Base class for field_timer_jquery_countdown formatter.
 */
abstract class field_timer_jquery_countdown_base extends field_timer_jquery_countdown_js_base {

  /**
   * @inheritdoc
   */
  final public function getFormatterDefinition() {
    return array (
      'label' => t('jQuery Countdown'),
      'description' => t('Use jQuery Countdown plugin to display countdown and timer.'),
    );
  }


  /**
   * @inheritdoc
   */
  public function getDefaultSettings() {
    return array(
      'type' => 'auto',
      'regional' => 'en',
      'format' => 'dHMS',
      'layout' => '',
      'compact' => 0,
      'significant' => 0,
      'timeSeparator' => ':',
      'padZeroes' => 0,
    );
  }
  
  /**
   * @inheritdoc
   */
  public function getSettingsSummary($settings, $field) {
    $summary = array();
    $types = $this->formatterTypes();
    $regions = $this->formatterRegions();

    $summary[] = t('Type: %type', array('%type' => $types[$settings['type']]));
    $summary[] = t('Region: %regional', array('%regional' => $regions[$settings['regional']]));
    $summary[] = t('Format: %format', array('%format' => $settings['format']));
    $summary[] = t('Layout: %layout', array('%layout' => $settings['layout']));
    $summary[] = t('Compact: %compact', array('%compact' => $settings['compact'] ? t('Yes') : t('No')));
    $summary[] = t('Significant: %significant', array('%significant' => $settings['significant']));
    $summary[] = t('Time separator: %timeSeparator', array('%timeSeparator' => $settings['timeSeparator']));
    $summary[] = t('Pad with zeroes: %padZeroes', array('%padZeroes' => $settings['padZeroes'] ? t('Yes') : t('No')));

    return $summary;
  }
  
  /**
   * @inheritdoc
   */
  public function getSettingsForm($settings, $field) {
    $settings_form = array();
    $settings_form['type'] = array(
      '#type' => 'select',
      '#title' => t('Type'),
      '#options' => $this->formatterTypes(),
      '#default_value' => $settings['type'],
    );

    $settings_form['regional'] = array(
      '#type' => 'select',
      '#title' => t('Region'),
      '#default_value' => $settings['regional'],
      '#options' => $this->formatterRegions(),
    );

    $settings_form['format'] = array(
      '#type' => 'textfield',
      '#title' => t('Format'),
      '#default_value' => $settings['format'],
      '#description' => t("See !documentation for this parameter.", array(
        '!documentation' => l(t('documentation'),'http://keith-wood.name/countdownRef.html', array('fragment' => 'format'))
      )),
    );

    $settings_form['layout'] = array(
      '#type' => 'textarea',
      '#rows' => 3,
      '#cols' => 60,
      '#title' => t('Layout'),
      '#defualt_value' => $settings['layout'],
      '#description' => t("See !documentation for this parameter.", array(
        '!documentation' => l(t('documentation'), 'http://keith-wood.name/countdownRef.html', array('fragment' => 'layout'))
      )),
    );

    $settings_form['compact'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display in compact format'),
      '#default_value' => $settings['compact'],
    );

    $settings_form['significant'] = array(
      '#type' => 'select',
      '#title' => t('Significant'),
      '#options' => range(0, 7),
      '#default_value' => $settings['significant'],
    );

    $settings_form['timeSeparator'] = array(
      '#type' => 'textfield',
      '#title' => t('Time separator'),
      '#default_value' => $settings['timeSeparator'],
    );

    $settings_form['padZeroes'] = array(
      '#type' => 'checkbox',
      '#title' => t('Pad with zeroes'),
      '#default_value' => $settings['padZeroes'],
    );

    return $settings_form;
  }
  
  /**
   * @inheritdoc
   */
  public function renderItems($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    if (!$this->loadLibrary()) {
      return array();
    }

    $info = entity_get_info($entity_type);
    $id_key = $info['entity keys']['id'];
    $elements = array();

    foreach ($items as $delta => $item) {
      $elements[$delta] = array(
        '#type' => 'markup',
        '#markup' => '<span id="jquery-countdown-' . $entity_type . '_'
        . $entity->{$id_key} . '_' . $field['field_name'] . '_' . $delta
        . '" class="jquery-countdown"></span>',
      );
    }

    $this->loadJSandCSS($entity_type, $entity, $field, $instance, $langcode, $items, $display);

    return $elements;
  }
  
  /**
   * @inheritdoc
   */
  public function getFormatterName() {
    return 'field_timer_jquery_count_down';
  }
  
  /**
   * @inheritdoc
   */
  public function getPluginName() {
    return 'jquery.countdown';
  }
  
  protected function formatterRegions() {
    return array(
      'sq' => t('Albanian'),
      'ar' => t('Arabic'),
      'hy' => t('Armenian'),
      'bn' => t('Bengali/Bangla'),
      'bs' => t('Bosnian'),
      'bg' => t('Bulgarian'),
      'my' => t('Burmese'),
      'ca' => t('Catalan'),
      'zh-CN' => t('Chinese/Simplified'),
      'zh-TW' => t('Chinese/Traditional'),
      'hr' => t('Croatian'),
      'cs' => t('Czech'),
      'da' => t('Danish'),
      'nl' => t('Dutch'),
      'et' => t('Estonian'),
      'en' => t('English'),
      'fa' => t('Farsi/Persian'),
      'fi' => t('Finnish'),
      'fo' => t('Faroese'),
      'fr' => t('French'),
      'gl' => t('Galician'),
      'de' => t('German'),
      'el' => t('Greek'),
      'gu' => t('Gujarati'),
      'he' => t('Hebrew'),
      'hu' => t('Hungarian'),
      'id' => t('Indonesian'),
      'is' => t('Icelandic'),
      'it' => t('Italian'),
      'ja' => t('Japanese'),
      'kn' => t('Kannada'),
      'ko' => t('Korean'),
      'lv' => t('Latvian'),
      'lt' => t('Lithuanian'),
      'ml' => t('Malayalam'),
      'ms' => t('Malaysian'),
      'nb' => t('Norvegian'),
      'pl' => t('Polish'),
      'pt-BR' => t('Portuguese/Brazilian'),
      'ro' => t('Romanian'),
      'ru' => t('Russian'),
      'sr' => t('Serbian'),
      'sk' => t('Slovak'),
      'sl' => t('Slovenian'),
      'es' => t('Spanish'),
      'sv' => t('Swedish'),
      'th' => t('Thai'),
      'tr' => t('Turkish'),
      'uk' => t('Ukrainian'),
      'ur' => t('Urdu'),
      'uz' => t('Uzbek'),
      'vi' => t('Vietnamese'),
      'cy' => t('Welsh'),
    );
  }
}

/**
 * Class for field_timer_jquery_countdown_formatter and field_timer field type.
 */
class field_timer_jquery_countdown_field_timer extends field_timer_jquery_countdown_base {
  
  /**
   * @inheritdoc
   */
  public function getFieldType() {
    return 'field_timer';
  }

  /**
   * @inheritdoc
   */
  protected function getTimestamp($item, $field, $key = 'timestamp') {
    return $item[$key];
  }
}

/**
 * Base class for field_timer_jquery_countdown formatter and date field types.
 */
abstract class field_timer_jquery_countdown_date_field_base extends field_timer_jquery_countdown_base {
  /**
   * @inheritdoc
   */
  public function getDefaultSettings() {
    return parent::getDefaultSettings() + array(
      'title' => '',
      'title2' => '',
      'type2' => 'auto',
      'regional2' => 'en',
      'format2' => 'dHMS',
      'layout2' => '',
      'compact2' => 0,
      'significant2' => 0,
      'timeSeparator2' => ':',
      'padZeroes2' => 0,
      'fromto' => 'both'
    );
  }
  
  /**
   * @inheritdoc
   */
  public function getSettingsSummary($settings, $field) {
    $summary = array();
    if (!empty($field['settings']['todate'])) {
      $types = $this->formatterTypes();
      $regions = $this->formatterRegions();
      $displays = $this->formatterDisplays();
      $summary[] = t('Title for Start date: %title', array('%title' => $settings['title']));
      $summary[] = t('Type for Start date: %type', array('%type' => $types[$settings['type']]));
      $summary[] = t('Region for Start date: %regional', array('%regional' => $regions[$settings['regional']]));
      $summary[] = t('Format for Start date: %format', array('%format' => $settings['format']));
      $summary[] = t('Layout for Start date: %layout', array('%layout' => $settings['layout']));
      $summary[] = t('Compact for Start date: %compact', array('%compact' => $settings['compact'] ? t('Yes') : t('No')));
      $summary[] = t('Significant for Start date: %significant', array('%significant' => $settings['significant']));
      $summary[] = t('Time separator for Start date: %timeSeparator', array('%timeSeparator' => $settings['timeSeparator']));
      $summary[] = t('Pad with zeroes for Start date: %padZeroes', array('%padZeroes' => $settings['padZeroes'] ? t('Yes') : t('No')));
      $summary[] = t('Title for End date: %title', array('%title' => $settings['title2']));
      $summary[] = t('Type for End date: %type', array('%type' => $types[$settings['type2']]));
      $summary[] = t('Region for End date: %regional', array('%regional' => $regions[$settings['regional2']]));
      $summary[] = t('Format for End date: %format', array('%format' => $settings['format2']));
      $summary[] = t('Layout for End date: %layout', array('%layout' => $settings['layout2']));
      $summary[] = t('Compact for End date: %compact', array('%compact' => $settings['compact2'] ? t('Yes') : t('No')));
      $summary[] = t('Significant for End date: %significant', array('%significant' => $settings['significant2']));
      $summary[] = t('Time separator for End date: %timeSeparator', array('%timeSeparator' => $settings['timeSeparator2']));
      $summary[] = t('Pad with zeroes for End date: %padZeroes', array('%padZeroes' => $settings['padZeroes2'] ? t('Yes') : t('No')));
      $summary[] = t('Display: %display', array('%display' => $displays[$settings['fromto']]));
    }
    else {
      $summary[] = t('Title: %title', array('%title' => $settings['title']));
      $summary = array_merge($summary, parent::getSettingsSummary($settings, $field));
    }

    return $summary;
  }
  
  /**
   * @inheritdoc
   */
  public function getSettingsForm($settings, $field) {
    $settings_form = parent::getSettingsForm($settings, $field);
    
    $settings_form['title'] = array(
      '#type' => 'textfield',
      '#title' => empty($field['settings']['todate']) ? t('Title') : t('Title for Start date'),
      '#default_value' => $settings['title'],
      '#weight' => -10,
    );
    
    if (!empty($field['settings']['todate'])) {
      $settings_form['type']['#title'] = t('Type for Start date');
      $settings_form['regional']['#title'] = t('Region for Start date');
      $settings_form['format']['#title'] = t('Format for Start date');
      $settings_form['layout']['#title'] = t('Layout for Start date');
      $settings_form['compact']['#title'] = t('Display Start date in compact format');
      $settings_form['significant']['#title'] = t('Significant for Start date');
      $settings_form['timeSeparator']['#title'] = t('Time separator for Start date');
      $settings_form['padZeroes']['#title'] = t('Pad with zeroes for Start date');
      
      $settings_form['title2'] = array(
        '#type' => 'textfield',
        '#title' => t('Title for End date'),
        '#default_value' => $settings['title2'],
      );
      
      $settings_form['type2'] = array(
        '#type' => 'select',
        '#title' => t('Type for End date'),
        '#options' => $this->formatterTypes(),
        '#default_value' => $settings['type2'],
      );

      $settings_form['regional2'] = array(
        '#type' => 'select',
        '#title' => t('Region for End date'),
        '#default_value' => $settings['regional2'],
        '#options' => $this->formatterRegions(),
      );

      $settings_form['format2'] = array(
        '#type' => 'textfield',
        '#title' => t('Format for End date'),
        '#default_value' => $settings['format2'],
        '#description' => t("See !documentation for this parameter.", array(
          '!documentation' => l(t('documentation'),'http://keith-wood.name/countdownRef.html', array('fragment' => 'format'))
        )),
      );

      $settings_form['layout2'] = array(
        '#type' => 'textarea',
        '#rows' => 3,
        '#cols' => 60,
        '#title' => t('Layout for End date'),
        '#defualt_value' => $settings['layout2'],
        '#description' => t("See !documentation for this parameter.", array(
          '!documentation' => l(t('documentation'), 'http://keith-wood.name/countdownRef.html', array('fragment' => 'layout'))
        )),
      );

      $settings_form['compact2'] = array(
        '#type' => 'checkbox',
        '#title' => t('Display End date in compact format'),
        '#default_value' => $settings['compact2'],
      );

      $settings_form['significant2'] = array(
        '#type' => 'select',
        '#title' => t('Significant for End date'),
        '#options' => range(0, 7),
        '#default_value' => $settings['significant2'],
      );

      $settings_form['timeSeparator2'] = array(
        '#type' => 'textfield',
        '#title' => t('Time separator for End date'),
        '#default_value' => $settings['timeSeparator2'],
      );

      $settings_form['padZeroes2'] = array(
        '#type' => 'checkbox',
        '#title' => t('Pad with zeroes for End date'),
        '#default_value' => $settings['padZeroes2'],
      );
      
      $settings_form['fromto'] = array(
        '#type' => 'select',
        '#title' => t('Display'),
        '#options' => $this->formatterDisplays(),
        '#default_value' => $settings['fromto'],
      );
    }
    
    return $settings_form;
  }

  /**
   * @inheritdoc
   */
  public function renderItems($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    $settings = $display['settings'];
    $elements = ($settings['fromto'] != 'to') ? parent::renderItems($entity_type, $entity, $field, $instance, $langcode, $items, $display) : array();
    $info = entity_get_info($entity_type);
    $id_key = $info['entity keys']['id'];

    foreach ($items as $delta => $item) {
      if (!empty($settings['title']) && $settings['fromto'] != 'to') {
        $elements[$delta]['#markup'] = '<span class="field-timer-start-date-title">'
        . $settings['title'] . '</span> ' . $elements[$delta]['#markup'];
      }
      if ($settings['fromto'] != 'from' && !empty($field['settings']['todate'])) {
        $title = (!empty($settings['title2'])) ? '<span class="field-timer-end-date-title">' . $settings['title2'] . '</span> ' : '';
        $time = '<span id="jquery-countdown-' . $entity_type . '_'
        . $entity->{$id_key} . '_' . $field['field_name'] . '_' . $delta . '_2'
        . '" class="jquery-countdown"></span>';
        if ($settings['fromto'] == 'to') {
          $elements[$delta] = array(
            '#type' => 'markup',
            '#markup' => $title . $time,
          );
        }
        else {
          $elements[$delta]['#markup'] .= ' ' . $title . $time;
        }
      }
    }

    return $elements;
  }
  
  /**
   * @inheritdoc
   */
  protected function generateJSSettings($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    $js_settings = parent::generateJSSettings($entity_type, $entity, $field, $instance, $langcode, $items, $display);
    if (!empty($field['settings']['todate'])) {
      $info = entity_get_info($entity_type);
      $id_key = $info['entity keys']['id'];
      $settings = $display['settings'];

      $settings2 = array();
      foreach ($settings as $name => $value) {
        if (substr($name, strlen($name - 2)) == '2') {
          $settings2[substr($name, 0, strlen($name) - 1)] = $value;
        }
      }

      foreach ($items as $delta => $item) {
        $key = $entity_type . '_' . $entity->{$id_key} . '_' . $field['field_name'] . '_' . $delta . '_2';
        $timestamp = $this->getTimestamp($item, $field, 'value2');
        $js_settings[$key]['timestamp'] = $timestamp;
        $js_settings[$key]['options'] = $settings2;
        if ($settings['type2'] == 'timer' || ($settings['type2'] == 'auto' && $timestamp <= REQUEST_TIME)) {
          $js_settings[$key]['options']['until'] = FALSE;
          $js_settings[$key]['options']['since'] = TRUE;
        }
        elseif ($settings['type2'] == 'countdown' || ($settings['type2'] == 'auto' && $timestamp > REQUEST_TIME)) {
          $js_settings[$key]['options']['until'] = TRUE;
          $js_settings[$key]['options']['since'] = FALSE;
        }
        $js_settings[$key]['plugin'] = $this->getPluginName();
      }
    }
    
    return $js_settings;
  }

  protected function formatterDisplays() {
    return array(
      'both' => t('Both Start and End dates'),
      'from' => t('Start date only'),
      'to' => t('End date only'),
    );
  }
}

/**
 * Class for field_timer_jquery_countdown formatter and datestamp field type.
 */
class field_timer_jquery_countdown_datestamp extends field_timer_jquery_countdown_date_field_base {
  
  /**
   * @inheritdoc
   */
  public function getFieldType() {
    return 'datestamp';
  }

  /**
   * @inheritdoc
   */
  protected function getTimestamp($item, $field, $key = 'value') {
    return $item[$key];
  }
}

/**
 * Class for field_timer_jquery_countdown formatter and date field type.
 */
class field_timer_jquery_countdown_date extends field_timer_jquery_countdown_date_field_base {
  
  /**
   * @inheritdoc
   */
  public function getFieldType() {
    return 'date';
  }

  /**
   * @inheritdoc
   */
  protected function getTimestamp($item, $field, $key = 'value') {
    $timezone_db = date_get_timezone_db($field['settings']['tz_handling']);
    $db_format = date_type_format($field['type']);
    $date = new DateObject($item[$key], $timezone_db, $db_format);
    return $date->getTimestamp();
  }
}

/**
 * Class for field_timer_jquery_countdown formatter and datetime field type.
 */
class field_timer_jquery_countdown_datetime extends field_timer_jquery_countdown_date_field_base {
  
  /**
   * @inheritdoc
   */
  public function getFieldType() {
    return 'datetime';
  }

  /**
   * @inheritdoc
   */
  protected function getTimestamp($item, $field, $key = 'value') {
    $timezone_db = date_get_timezone_db($field['settings']['tz_handling']);
    $db_format = date_type_format($field['type']);
    $date = new DateObject($item[$key], $timezone_db, $db_format);
    return $date->getTimestamp();
  }
}

/**
 * Base class for field_timer_jquery_countdown_led formatter.
 */
abstract class field_timer_jquery_countdown_led_base extends field_timer_jquery_countdown_js_base {
  
  /**
   * @inheritdoc
   */
  final public function getFormatterDefinition() {
    return array (
      'label' => t('jQuery Countdown LED'),
      'description' => t('Display timer and countdown as a LED.'),
    );
  }


  /**
   * @inheritdoc
   */
  public function getDefaultSettings() {
    return array(
      'type' => 'auto',
      'countdown_theme' => 'green',
      'max_count_of_days' => 2,
      'display_days' => 1,
      'display_hours' => 1,
      'display_minutes' => 1,
      'display_seconds' => 1,
    );
  }
  
  /**
   * @inheritdoc
   */
  public function getSettingsSummary($settings, $field) {
    $summary = array();
    $types = $this->formatterTypes();
    $themes = $this->formatterThemes();
    $days = $this->formatterDays();

    $summary[] = t('Type: %type', array('%type' => $types[$settings['type']]));
    $summary[] = t('Theme: %theme', array('%theme' => $themes[$settings['countdown_theme']]));
    $summary[] = t('Display days: %display_days', array('%display_days' => $settings['display_days'] ? t('Yes') : t('No')));
    if ($settings['display_days']) {
      $summary[] = t('Maximum count of days: %max_count_of_days', array('%max_count_of_days' => $days[$settings['max_count_of_days']]));
    }
    $summary[] = t('Display hours: %display_hours', array('%display_hours' => $settings['display_hours'] ? t('Yes') : t('No')));
    $summary[] = t('Display minutes: %display_minutes', array('%display_minutes' => $settings['display_minutes'] ? t('Yes') : t('No')));
    $summary[] = t('Display seconds: %display_seconds', array('%display_seconds' => $settings['display_seconds'] ? t('Yes') : t('No')));

    return $summary;
  }
  
  /**
   * @inheritdoc
   */
  public function getSettingsForm($settings, $field) {
    $settings_form = array();
    $settings_form['type'] = array(
      '#type' => 'select',
      '#title' => t('Type'),
      '#options' => $this->formatterTypes(),
      '#default_value' => $settings['type'],
    );

    $settings_form['countdown_theme'] = array(
      '#type' => 'select',
      '#title' => t('Theme'),
      '#options' => $this->formatterThemes(),
      '#default_value' => $settings['countdown_theme'],
    );

    $settings_form['display_days'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display days'),
      '#default_value' => $settings['display_days'],
      '#attributes' => array('class' => array('field-timer-dislay-days')),
    );

    $settings_form['max_count_of_days'] = array(
      '#type' => 'select',
      '#title' => t('Max count of days'),
      '#options' => $this->formatterDays(),
      '#default_value' => $settings['max_count_of_days'],
      '#states' => array(
        'invisible' => array(
          'input.field-timer-dislay-days' => array('checked' => FALSE),
        ),
      ),
    );

    $settings_form['display_hours'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display hours'),
      '#default_value' => $settings['display_hours'],
    );

    $settings_form['display_minutes'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display minutes'),
      '#default_value' => $settings['display_minutes'],
    );

    $settings_form['display_seconds'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display seconds'),
      '#default_value' => $settings['display_seconds'],
    );

    return $settings_form;
  }
  
  /**
   * @inheritdoc
   */
  public function renderItems($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    if (!$this->loadLibrary()) {
      return array();
    }

    $info = entity_get_info($entity_type);
    $id_key = $info['entity keys']['id'];
    $elements = array();
    $settings = $display['settings'];

    foreach ($items as $delta => $item) {
      $layout = '<div class="jquery-countdown-led-display-wrapper">';
      if ($settings['display_days']) {
        for ($i = $settings['max_count_of_days']; $i > 0; $i--) {
          $layout .= '<span class="%t% image{d1' . substr('000', 0, $i - 1) . '}"></span>';
        }
        $layout .= '<span class="%t% imageDay"></span><span class="%t% imageSpace"></span>';
      }
      if ($settings['display_hours']) {
        $layout .= '<span class="%t% image{h10}"></span><span class="%t% image{h1}"></span>';
        if ($settings['display_minutes'] || $settings['display_seconds']) {
          $layout .= '<span class="%t% imageSep"></span>';
        }
      }
      if ($settings['display_minutes']) {
        $layout .= '<span class="%t% image{m10}"></span><span class="%t% image{m1}"></span>';
        if ($settings['display_seconds']) {
          $layout .= '<span class="%t% imageSep"></span>';
        }
      }
      if ($settings['display_seconds']) {
        $layout .= '<span class="%t% image{s10}"></span><span class="%t% image{s1}"></span>';
      }

      $elements[$delta] = array(
        '#type' => 'markup',
        '#markup' => '<span id="jquery-countdown-led-' . $entity_type . '_'
        . $entity->{$id_key} . '_' . $field['field_name'] . '_' . $delta
        . '" class="jquery-countdown-led ' . $settings['countdown_theme'] . '">'
        . str_replace('%t%', $settings['countdown_theme'], $layout) . '</span>',
      );
    }

    $this->loadJSandCSS($entity_type, $entity, $field, $instance, $langcode, $items, $display);

    return $elements;
  }
  
  /**
   * @inheritdoc
   */
  protected function loadJSandCSS($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    parent::loadJSandCSS($entity_type, $entity, $field, $instance, $langcode, $items, $display);
    // Since we can't get path to image from CSS file, we add this rule here.
    $path = libraries_get_path('jquery.countdown');
    drupal_add_css('.jquery-countdown-led.green span{background: url(/' . $path . '/countdownLED.png) no-repeat 0px 0px;} .jquery-countdown-led.blue span{background: url(/' . $path . '/countdownGlowing.gif) no-repeat 0px 0px;}', 'inline');
  }

  /**
   * @inheritdoc
   */
  public function getFormatterName() {
    return 'field_timer_jquery_countdown_led';
  }
  
  /**
   * @inheritdoc
   */
  public function getPluginName() {
    return 'jquery.countdown.led';
  }
  
  protected function formatterThemes() {
    return array(
      'green' => t('Green'),
      'blue' => t('Blue'),
    );
  }
  
  protected function formatterDays() {
    return array(
      1 => 9,
      2 => 99,
      3 => 999,
      4 => 9999,
    );
  }
}

/**
 * Class for field_timer_jquery_countdown_led formatter and field_timer field type.
 */
class field_timer_jquery_countdown_led_field_timer extends field_timer_jquery_countdown_led_base {
  
  /**
   * @inheritdoc
   */
  public function getFieldType() {
    return 'field_timer';
  }

  /**
   * @inheritdoc
   */
  protected function getTimestamp($item, $field, $key = 'timestamp') {
    return $item[$key];
  }
}

/**
 * Base class for field_timer_jquery_countdown_led formatter and date field types.
 */
abstract class field_timer_jquery_countdown_led_date_field_base extends field_timer_jquery_countdown_led_base {
  
  /**
   * @inheritdoc
   */
  public function getDefaultSettings() {
    return parent::getDefaultSettings() + array(
      'title' => '',
      'title2' => '',
      'type2' => 'auto',
      'countdown_theme2' => 'green',
      'max_count_of_days2' => 2,
      'display_days2' => 1,
      'display_hours2' => 1,
      'display_minutes2' => 1,
      'display_seconds2' => 1,
      'fromto' => 'both',
    );
  }
  
  /**
   * @inheritdoc
   */
  public function getSettingsSummary($settings, $field) {
    $summary = array();
    if (!empty($field['settings']['todate'])) {
      $types = $this->formatterTypes();
      $themes = $this->formatterThemes();
      $days = $this->formatterDays();
      $displays = $this->formatterDisplays();

      $summary[] = t('Title for Start date: %title', array('%title' => $settings['title']));
      $summary[] = t('Type for Start date: %type', array('%type' => $types[$settings['type']]));
      $summary[] = t('Theme for Start date: %theme', array('%theme' => $themes[$settings['countdown_theme']]));
      $summary[] = t('Display days for Start date: %display_days', array('%display_days' => $settings['display_days'] ? t('Yes') : t('No')));
      if ($settings['display_days']) {
        $summary[] = t('Maximum count of days for Start date: %max_count_of_days', array('%max_count_of_days' => $days[$settings['max_count_of_days']]));
      }
      $summary[] = t('Display hours for Start date: %display_hours', array('%display_hours' => $settings['display_hours'] ? t('Yes') : t('No')));
      $summary[] = t('Display minutes for Start date: %display_minutes', array('%display_minutes' => $settings['display_minutes'] ? t('Yes') : t('No')));
      $summary[] = t('Display seconds for Start date: %display_seconds', array('%display_seconds' => $settings['display_seconds'] ? t('Yes') : t('No')));
      $summary[] = t('Title for End date: %title', array('%title' => $settings['title2']));
      $summary[] = t('Type for End date: %type', array('%type' => $types[$settings['type2']]));
      $summary[] = t('Theme for End date: %theme', array('%theme' => $themes[$settings['countdown_theme2']]));
      $summary[] = t('Display days for End date: %display_days', array('%display_days' => $settings['display_days2'] ? t('Yes') : t('No')));
      if ($settings['display_days2']) {
        $summary[] = t('Maximum count of days for End date: %max_count_of_days', array('%max_count_of_days' => $days[$settings['max_count_of_days2']]));
      }
      $summary[] = t('Display hours for End date: %display_hours', array('%display_hours' => $settings['display_hours2'] ? t('Yes') : t('No')));
      $summary[] = t('Display minutes for End date: %display_minutes', array('%display_minutes' => $settings['display_minutes2'] ? t('Yes') : t('No')));
      $summary[] = t('Display seconds for End date: %display_seconds', array('%display_seconds' => $settings['display_seconds2'] ? t('Yes') : t('No')));
      $summary[] = t('Display: %display', array('%display' => $displays[$settings['fromto']]));
    }
    else {
      $summary[] = t('Title: %title', array('%title' => $settings['title']));
      $summary = array_merge($summary, parent::getSettingsSummary($settings, $field));
    }

    return $summary;
  }

  /**
   * @inheritdoc
   */
  public function getSettingsForm($settings, $field) {
    $settings_form = parent::getSettingsForm($settings, $field);
    
    $settings_form['title'] = array(
      '#type' => 'textfield',
      '#title' => empty($field['settings']['todate']) ? t('Title') : t('Title for Start date'),
      '#default_value' => $settings['title'],
      '#weight' => -10,
    );
    
    if (!empty($field['settings']['todate'])) {
      $settings_form['type']['#title'] = t('Type for Start date');
      $settings_form['countdown_theme']['#title'] = t('Theme for Start date');
      $settings_form['display_days']['#title'] = t('Display days');
      $settings_form['max_count_of_days']['#title'] = t('Max count of days');
      $settings_form['display_hours']['#title'] = t('Display hours');
      $settings_form['display_minutes']['#title'] = t('Display minutes');
      $settings_form['display_seconds']['#title'] = t('Display seconds');
      
      $settings_form['title2'] = array(
        '#type' => 'textfield',
        '#title' => t('Title for End date'),
        '#default_value' => $settings['title2'],
      );
      
      $settings_form['type2'] = array(
        '#type' => 'select',
        '#title' => t('Type for End date'),
        '#options' => $this->formatterTypes(),
        '#default_value' => $settings['type2'],
      );

      $settings_form['countdown_theme2'] = array(
        '#type' => 'select',
        '#title' => t('Theme for End date'),
        '#options' => $this->formatterThemes(),
        '#default_value' => $settings['countdown_theme2'],
      );

      $settings_form['display_days2'] = array(
        '#type' => 'checkbox',
        '#title' => t('Display days for End date'),
        '#default_value' => $settings['display_days2'],
        '#attributes' => array('class' => array('field-timer-dislay-days-2')),
      );

      $settings_form['max_count_of_days2'] = array(
        '#type' => 'select',
        '#title' => t('Max count of days for End date'),
        '#options' => $this->formatterDays(),
        '#default_value' => $settings['max_count_of_days2'],
        '#states' => array(
          'invisible' => array(
            'input.field-timer-dislay-days-2' => array('checked' => FALSE),
          ),
        ),
      );

      $settings_form['display_hours2'] = array(
        '#type' => 'checkbox',
        '#title' => t('Display hours for End date'),
        '#default_value' => $settings['display_hours2'],
      );

      $settings_form['display_minutes2'] = array(
        '#type' => 'checkbox',
        '#title' => t('Display minutes for End date'),
        '#default_value' => $settings['display_minutes2'],
      );

      $settings_form['display_seconds2'] = array(
        '#type' => 'checkbox',
        '#title' => t('Display seconds for End date'),
        '#default_value' => $settings['display_seconds2'],
      );
      
      $settings_form['fromto'] = array(
        '#type' => 'select',
        '#title' => t('Display'),
        '#options' => $this->formatterDisplays(),
        '#default_value' => $settings['fromto'],
      );
    }

    return $settings_form;
  }
  
  /**
   * @inheritdoc
   */
  public function renderItems($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    $info = entity_get_info($entity_type);
    $id_key = $info['entity keys']['id'];
    $elements = parent::renderItems($entity_type, $entity, $field, $instance, $langcode, $items, $display);
    $settings = $display['settings'];

    foreach ($items as $delta => $item) {
      if (!empty($settings['title']) && $settings['fromto'] != 'to') {
        $elements[$delta]['#markup'] = '<span class="field-timer-start-date-title">'
        . $settings['title'] . '</span> ' . $elements[$delta]['#markup'];
      }
      if ($settings['fromto'] != 'from' && !empty($field['settings']['todate'])) {
        $title = (!empty($settings['title2'])) ? '<span class="field-timer-end-date-title">' . $settings['title2'] . '</span> ' : '';
        $layout = '<div class="jquery-countdown-led-display-wrapper">';
        if ($settings['display_days2']) {
          for ($i = $settings['max_count_of_days2']; $i > 0; $i--) {
            $layout .= '<span class="%t% image{d1' . substr('000', 0, $i - 1) . '}"></span>';
          }
          $layout .= '<span class="%t% imageDay"></span><span class="%t% imageSpace"></span>';
        }
        if ($settings['display_hours2']) {
          $layout .= '<span class="%t% image{h10}"></span><span class="%t% image{h1}"></span>';
          if ($settings['display_minutes2'] || $settings['display_seconds2']) {
            $layout .= '<span class="%t% imageSep"></span>';
          }
        }
        if ($settings['display_minutes2']) {
          $layout .= '<span class="%t% image{m10}"></span><span class="%t% image{m1}"></span>';
          if ($settings['display_seconds2']) {
            $layout .= '<span class="%t% imageSep"></span>';
          }
        }
        if ($settings['display_seconds2']) {
          $layout .= '<span class="%t% image{s10}"></span><span class="%t% image{s1}"></span>';
        }
        $layout .= '</div>';
        $time = '<span id="jquery-countdown-led-' . $entity_type . '_'
        . $entity->{$id_key} . '_' . $field['field_name'] . '_' . $delta
        . '_2" class="jquery-countdown-led ' . $settings['countdown_theme2'] . '">'
        . str_replace('%t%', $settings['countdown_theme2'], $layout) . '</span>';
        if ($settings['fromto'] == 'to') {
          $elements[$delta] = array(
            '#type' => 'markup',
            '#markup' => $title . $time,
          );
        }
        else {
          $elements[$delta]['#markup'] .= ' ' . $title . $time;
        }
      }
    }

    return $elements;
  }
  
  /**
   * @inheritdoc
   */
  protected function generateJSSettings($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    $js_settings = parent::generateJSSettings($entity_type, $entity, $field, $instance, $langcode, $items, $display);
    if (!empty($field['settings']['todate'])) {
      $info = entity_get_info($entity_type);
      $id_key = $info['entity keys']['id'];
      $settings = $display['settings'];

      $settings2 = array();
      foreach ($settings as $name => $value) {
        if (substr($name, strlen($name - 2)) == '2') {
          $settings2[substr($name, 0, strlen($name) - 1)] = $value;
        }
      }

      foreach ($items as $delta => $item) {
        $key = $entity_type . '_' . $entity->{$id_key} . '_' . $field['field_name'] . '_' . $delta . '_2';
        $timestamp = $this->getTimestamp($item, $field, 'value2');
        $js_settings[$key]['timestamp'] = $timestamp;
        $js_settings[$key]['options'] = $settings2;
        if ($settings['type2'] == 'timer' || ($settings['type2'] == 'auto' && $timestamp <= REQUEST_TIME)) {
          $js_settings[$key]['options']['until'] = FALSE;
          $js_settings[$key]['options']['since'] = TRUE;
        }
        elseif ($settings['type2'] == 'countdown' || ($settings['type2'] == 'auto' && $timestamp > REQUEST_TIME)) {
          $js_settings[$key]['options']['until'] = TRUE;
          $js_settings[$key]['options']['since'] = FALSE;
        }
        $js_settings[$key]['plugin'] = $this->getPluginName();
      }
    }
    
    return $js_settings;
  }
  
  protected function formatterDisplays() {
    return array(
      'both' => t('Both Start and End dates'),
      'from' => t('Start date only'),
      'to' => t('End date only'),
    );
  }
}

/**
 * Class for field_timer_jquery_countdown formatter and datestamp field type.
 */
class field_timer_jquery_countdown_led_datestamp extends field_timer_jquery_countdown_led_date_field_base {
  
  /**
   * @inheritdoc
   */
  public function getFieldType() {
    return 'datestamp';
  }

  /**
   * @inheritdoc
   */
  protected function getTimestamp($item, $field, $key = 'value') {
    return $item[$key];
  }
}

/**
 * Class for field_timer_jquery_countdown formatter and date field type.
 */
class field_timer_jquery_countdown_led_date extends field_timer_jquery_countdown_led_date_field_base {
  
  /**
   * @inheritdoc
   */
  public function getFieldType() {
    return 'date';
  }

  /**
   * @inheritdoc
   */
  protected function getTimestamp($item, $field, $key = 'value') {
    $timezone_db = date_get_timezone_db($field['settings']['tz_handling']);
    $db_format = date_type_format($field['type']);
    $date = new DateObject($item[$key], $timezone_db, $db_format);
    return $date->getTimestamp();
  }
}

/**
 * Class for field_timer_jquery_countdown formatter and datetime field type.
 */
class field_timer_jquery_countdown_led_datetime extends field_timer_jquery_countdown_led_date_field_base {
  
  /**
   * @inheritdoc
   */
  public function getFieldType() {
    return 'datetime';
  }

  /**
   * @inheritdoc
   */
  protected function getTimestamp($item, $field, $key = 'value') {
    $timezone_db = date_get_timezone_db($field['settings']['tz_handling']);
    $db_format = date_type_format($field['type']);
    $date = new DateObject($item[$key], $timezone_db, $db_format);
    return $date->getTimestamp();
  }
}
