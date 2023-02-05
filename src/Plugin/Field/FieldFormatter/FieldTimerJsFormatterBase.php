<?php

namespace Drupal\field_timer\Plugin\Field\FieldFormatter;

use Drupal\Component\Utility\Crypt;
use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\Field\FieldItemInterface;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Field\FormatterBase;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Implementation of formatters that uses JavaScript to render timer/countdown.
 */
abstract class FieldTimerJsFormatterBase extends FormatterBase {

  /**
   * Plugin name used to render timer/countdown widget.
   */
  const LIBRARY_NAME = '';

  /**
   * Key used by js code to determine how to initialize the timer/countdown.
   */
  const JS_KEY = '';

  /**
   * Stores set of unique html ids for current items.
   *
   * @var array
   */
  protected $itemKeys;

  protected ConfigFactoryInterface $configFactory;

  /**
   * @param $plugin_id
   * @param $plugin_definition
   * @param \Drupal\Core\Field\FieldDefinitionInterface $field_definition
   * @param array $settings
   * @param $label
   * @param $view_mode
   * @param array $third_party_settings
   * @param \Drupal\Core\Config\ConfigFactoryInterface|NULL $configFactory
   */
  public function __construct($plugin_id, $plugin_definition, FieldDefinitionInterface $field_definition, array $settings, $label, $view_mode, array $third_party_settings, ConfigFactoryInterface $configFactory = NULL) {
    parent::__construct($plugin_id, $plugin_definition, $field_definition, $settings, $label, $view_mode, $third_party_settings);

    if (!$configFactory) {
      $message = 'Calling ' . __METHOD__ . ' without the $configFactory '
        . 'argument is deprecated in field_timer:2.1.0 and it will be required '
        . 'in field_timer:3.0.0. See https://www.drupal.org/node/3078162';
      @trigger_error($message, E_USER_DEPRECATED);
      $configFactory = \Drupal::configFactory();
    }

    $this->configFactory = $configFactory;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static($plugin_id, $plugin_definition, $configuration['field_definition'], $configuration['settings'], $configuration['label'], $configuration['view_mode'], $configuration['third_party_settings']);
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    $elements['#attached']['library'][] = $this->getLibraryName();
    $elements['#attached']['library'][] = 'field_timer/init';
    $elements['#attached']['drupalSettings']['field_timer'] = $this->generateJsSettings($items, $langcode);
    $elements['#cache']['tags'] = $this->configFactory->get('field_timer.config')
      ->getCacheTags();

    return $elements;
  }

  /**
   * Generates unique ids for the field items.
   *
   * @param \Drupal\Core\Field\FieldItemListInterface $items
   *   The field items.
   *
   * @return array
   *   Array of ids keyed by field item delta.
   */
  protected function getItemKeys(FieldItemListInterface $items) {
    $entity = $items->getEntity();
    if (!isset($this->itemKeys[$entity->getEntityTypeId()][$entity->id()][$items->getFieldDefinition()->getName()])) {
      $entity = $items->getEntity();

      $this->itemKeys = [];
      foreach ($items as $delta => $item) {
        $this->itemKeys[$entity->getEntityTypeId()][$entity->id()][$items->getFieldDefinition()->getName()][$delta] = implode('-', [
          $entity->getEntityTypeId(),
          $entity->id(),
          $items->getFieldDefinition()->getName(),
          $delta,
          Crypt::randomBytesBase64(8),
        ]);
      }
    }

    return $this->itemKeys[$entity->getEntityTypeId()][$entity->id()][$items->getFieldDefinition()->getName()] ?? [];
  }

  /**
   * Generates JS settings for the field.
   *
   * @param \Drupal\Core\Field\FieldItemListInterface $items
   *   Field items.
   * @param string $langcode
   *   Langcode value.
   *
   * @return array
   *   Array of JS settings to be used to initialize timer/countdown widget.
   */
  protected function generateJsSettings(FieldItemListInterface $items, $langcode) {
    $keys = $this->getItemKeys($items);
    $js_settings = [];

    foreach ($items as $delta => $item) {
      $timestamp = $this->getTimestamp($item);
      if ($timestamp !== NULL) {
        $js_settings[$keys[$delta]]['settings'] = $this->preparePluginSettings($item, $langcode);
        $js_settings[$keys[$delta]]['plugin'] = static::JS_KEY;
      }
    }

    return $js_settings;
  }

  /**
   * Retrieves timestamp from field item.
   *
   * @param \Drupal\Core\Field\FieldItemInterface $item
   *   Field item.
   *
   * @return int|null
   *   Datetime field item timestamp.
   */
  protected function getTimestamp(FieldItemInterface $item) {
    if (!empty($item->date)) {
      return $item->date->getTimestamp();
    }

    return NULL;
  }

  /**
   * @return string
   */
  protected function getLibraryName(): string {
    $name = 'field_timer/' . static::LIBRARY_NAME;
    $assetSource = $this->configFactory->get('field_timer.config')
      ->get('asset_source');
    if ($assetSource && $assetSource !== 'local') {
      $name .= '-' . $assetSource;
    }

    return $name;
  }

  /**
   * Prepares array of settings used to initialize jQuery plugin.
   *
   * @param \Drupal\Core\Field\FieldItemInterface $item
   *   Field item.
   * @param string $langcode
   *   Langcode value.
   *
   * @return array
   *   Array of key-value pairs.
   */
  abstract protected function preparePluginSettings(FieldItemInterface $item, $langcode);

}
