<?php

namespace Drupal\field_timer\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;

class ConfigForm extends ConfigFormBase {

  /**
   * @inheritDoc
   */
  protected function getEditableConfigNames() {
    return ['field_timer.config'];
  }

  /**
   * @inheritDoc
   */
  public function getFormId() {
    return 'field_timer_config_form';
  }

  /**
   * @inheritDoc
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $form['asset_source'] = [
      '#type' => 'radios',
      '#title' => $this->t('Asset source'),
      '#options' => [
        'local' => $this->t('Local directory (web/libraries)'),
        'js-delivr' => $this->t('CDN (jsDelivr)')
      ],
      '#default_value' => $this->config('field_timer.config')
        ->get('asset_source'),
    ];

    return parent::buildForm($form, $form_state);
  }

  /**
   * @inheritDoc
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $this->config('field_timer.config')
      ->set('asset_source', $form_state->getValue('asset_source'))
      ->save();

    parent::submitForm($form, $form_state);
  }

}
