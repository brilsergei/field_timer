<?php
/**
 * Created by PhpStorm.
 * User: sergei
 * Date: 29.11.15
 * Time: 19:05
 */

namespace Drupal\field_timer\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;


/**
 * Plugin implementation of the 'field_timer_countdown' formatter.
 *
 * @FieldFormatter(
 *   id = "field_timer_countdown",
 *   label = @Translation("jQuery Countdown"),
 *   field_types = {
 *     "datetime"
 *   }
 * )
 */
class FieldTimerCountdownFormatter extends FieldTimerCountdownFormatterBase {

  /**
   * {@inheritdoc}
   */
  const JS_KEY = 'jquery.countdown';

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = array(
      'regional' => 'en',
      'format' => 'dHMS',
      'layout' => '',
      'compact' => 0,
      'significant' => 0,
      'timeSeparator' => ':',
      'padZeroes' => 0,
    ) + parent::defaultSettings();

    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = parent::viewElements($items, $langcode);

    $elements['#attached']['library'][] = 'field_timer/' . static::LIBRARY_NAME . '.' . $this->getSetting('regional');

    $ids = $this->generateIds($items);

    foreach ($items as $delta => $item) {
      $elements[$delta] = array(
        '#markup' => '<span id="' . $ids[$delta] . '" class="field-timer-jquery-countdown"'
          . ' data-timestamp="' . $this->getTimestamp($item) . '"></span>',
      );
    }

    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $form = parent::settingsForm($form, $form_state);

    $form['regional'] = array(
      '#type' => 'select',
      '#title' => $this->t('Region'),
      '#default_value' => $this->getSetting('regional'),
      '#options' => $this->regionOptions(),
    );

    $form['format'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('Format'),
      '#default_value' => $this->getSetting('format'),
      '#description' => $this->t('See <a href=":url" target="_blank">documentation</a> for this parameter.', array(
        ':url' => $this->getDocumentationLink(array('fragment' => 'format')),
      )),
    );

    $form['layout'] = array(
      '#type' => 'textarea',
      '#rows' => 3,
      '#cols' => 60,
      '#title' => $this->t('Layout'),
      '#default_value' => $this->getSetting('layout'),
      '#description' => $this->t('See <a href=":url" target="_blank">documentation</a> for this parameter.', array(
        ':url' => $this->getDocumentationLink(array('fragment' => 'layout')),
      )),
    );

    $form['compact'] = array(
      '#type' => 'checkbox',
      '#title' => $this->t('Display in compact format'),
      '#default_value' => $this->getSetting('compact'),
    );

    $form['significant'] = array(
      '#type' => 'select',
      '#title' => $this->t('Granularity'),
      '#options' => range(0, 7),
      '#default_value' => $this->getSetting('significant'),
    );

    $form['timeSeparator'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('Time separator'),
      '#default_value' => $this->getSetting('timeSeparator'),
    );

    $form['padZeroes'] = array(
      '#type' => 'checkbox',
      '#title' => $this->t('Pad with zeroes'),
      '#default_value' => $this->getSetting('padZeroes'),
    );

    return $form;
  }

  /**
   * @inheritdoc
   */
  public function settingsSummary() {
    $summary = parent::settingsSummary();

    $region = $this->getSetting('regional');
    $summary[] = $this->t('Region: %regional', array('%regional' => $this->regionOptions()[$region]));
    $summary[] = $this->t('Format: %format', array('%format' => $this->getSetting('format')));
    $summary[] = $this->t('Layout: %layout', array('%layout' => $this->getSetting('layout')));
    $summary[] = $this->t('Compact: %compact', array('%compact' => $this->getSetting('compact') ? $this->t('Yes') : $this->t('No')));
    $summary[] = $this->t('Granularity: %significant', array('%significant' => $this->getSetting('significant')));
    $summary[] = $this->t('Time separator: %timeSeparator', array('%timeSeparator' => $this->getSetting('timeSeparator')));
    $summary[] = $this->t('Pad with zeroes: %padZeroes', array('%padZeroes' => $this->getSetting('padZeroes') ? $this->t('Yes') : $this->t('No')));

    return $summary;
  }

  protected function regionOptions() {
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