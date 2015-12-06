<?php

/**
 * @file
 * Contains Drupal\field_timer\Plugin\Field\FieldFormatter\FieldTimerSimpleTextFormatter.
 */

namespace Drupal\field_timer\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\datetime\Plugin\Field\FieldFormatter\DateTimeTimeAgoFormatter;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of the 'field_timer_simple_text' formatter.
 *
 * @FieldFormatter(
 *   id = "field_timer_simple_text",
 *   label = @Translation("Text timer or countdown"),
 *   field_types = {
 *     "datetime"
 *   }
 * )
 */
class FieldTimerSimpleTextFormatter extends DateTimeTimeAgoFormatter {

  /**
   * Formatter types.
   */
  const TYPE_AUTO = 'auto';
  const TYPE_TIMER = 'timer';
  const TYPE_COUNTDOWN = 'countdown';

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = array(
        'type' => static::TYPE_AUTO,
      ) + parent::defaultSettings();

    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = parent::viewElements($items, $langcode);
    $type = $this->getSetting('type');

    foreach ($items as $delta => $item) {
      switch ($type) {
        case static::TYPE_TIMER:
          if ($item->date->getTimestamp() < REQUEST_TIME) {
            unset($elements[$delta]);
          }
          break;
        case static::TYPE_COUNTDOWN:
          if ($item->date->getTimestamp() > REQUEST_TIME) {
            unset($elements[$delta]);
          }
      }
    }

    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $form = parent::settingsForm($form, $form_state);

    $form['type'] = array(
      '#type' => 'select',
      '#title' => $this->t('Type'),
      '#default_value' => $this->getSetting('type'),
      '#options' => $this->typeOptions(),
      '#description' => $this->t('Switch timer/countdown automatically or disable it.'),
    );

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = parent::settingsSummary();

    $type = $this->getSetting('type');
    $summary[] = $this->t('Type: @type', array('@type' => $this->typeOptions()[$type]));

    return $summary;
  }

  protected function typeOptions() {
    return array(
      static::TYPE_AUTO => $this->t('Auto'),
      static::TYPE_TIMER => $this->t('Timer'),
      static::TYPE_COUNTDOWN => $this->t('Countdown'),
    );
  }

}
