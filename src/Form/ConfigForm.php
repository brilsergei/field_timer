<?php

namespace Drupal\field_timer\Form;

use Drupal\Core\Asset\LibraryDiscoveryInterface;
use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

class ConfigForm extends ConfigFormBase {

  /**
   * Library discovery service
   *
   * @var \Drupal\Core\Asset\LibraryDiscoveryInterface
   */
  private $libraryDiscovery;

  /**
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   * @param \Drupal\Core\Asset\LibraryDiscoveryInterface $libraryDiscovery
   */
  public function __construct(ConfigFactoryInterface $config_factory, LibraryDiscoveryInterface $libraryDiscovery) {
    parent::__construct($config_factory);

    $this->libraryDiscovery = $libraryDiscovery;
  }

  /**
   * @inheritDoc
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('config.factory'),
      $container->get('library.discovery')
    );
  }

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
    $this->libraryDiscovery->clearCachedDefinitions();

    parent::submitForm($form, $form_state);
  }

}
