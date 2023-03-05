<?php

namespace Drupal\Tests\field_timer\Functional;

use Drupal\Core\Datetime\DrupalDateTime;
use Drupal\Core\Datetime\Entity\DateFormat;
use Drupal\Core\Entity\Entity\EntityFormDisplay;
use Drupal\Core\Entity\Entity\EntityViewDisplay;
use Drupal\field\Entity\FieldConfig;
use Drupal\field\Entity\FieldStorageConfig;
use Drupal\Tests\BrowserTestBase;

class AssetSourceTest extends BrowserTestBase {

  /**
   * A field storage to use in this test class.
   *
   * @var \Drupal\field\Entity\FieldStorageConfig
   */
  protected $fieldStorage;

  /**
   * The field used in this test class.
   *
   * @var \Drupal\field\Entity\FieldConfig
   */
  protected $field;

  /**
   * @var string
   */
  protected $fieldName = 'field_timer';

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['field_timer', 'entity_test', 'field_ui'];

  /**
   * {@inheritdoc}
   */
  protected $defaultTheme = 'stark';

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    $web_user = $this->drupalCreateUser([
      'access administration pages',
      'view test entity',
      'administer entity_test content',
      'administer entity_test fields',
      'administer entity_test display',
      'administer entity_test form display',
      'view the administration theme',
      'administer site configuration',
    ]);
    $this->drupalLogin($web_user);

    $type = 'datetime';
    $widgetType = 'datetime_default';

    $this->fieldStorage = FieldStorageConfig::create([
      'field_name' => $this->fieldName,
      'entity_type' => 'entity_test',
      'type' => $type,
    ]);
    $this->fieldStorage->save();
    $this->field = FieldConfig::create([
      'field_storage' => $this->fieldStorage,
      'bundle' => 'entity_test',
      'required' => TRUE,
    ]);
    $this->field->save();

    EntityFormDisplay::load('entity_test.entity_test.default')
      ->setComponent($this->fieldName, ['type' => $widgetType])
      ->save();

    EntityViewDisplay::create([
      'targetEntityType' => $this->field->getTargetEntityTypeId(),
      'bundle' => $this->field->getTargetBundle(),
      'mode' => 'full',
      'status' => TRUE,
    ])->save();
  }

  /**
   * @dataProvider assetSourcesDataProvider
   */
  public function testAssetSources(string $formatter, array $configs, array $assets) {
    // Configure display.
    $this->drupalGet('entity_test/structure/entity_test/display/full');
    $fieldFormatterEdit = [
      'fields[' . $this->fieldName . '][region]' => 'content',
      'fields[' . $this->fieldName . '][type]' => $formatter,
    ];
    $this->submitForm($fieldFormatterEdit, 'Save');

    // Configure formatter if required.
    if (count($configs) > 0) {
      $this->submitForm([], $this->fieldName . '_settings_edit');
      $edit = [];
      $keyPrefix = 'fields[' . $this->fieldName
        . '][settings_edit_form][settings][';
      foreach ($configs as $config => $value) {
        $edit[$keyPrefix . $config . ']'] = $value;
      }
      $this->submitForm($edit, 'Update');
      $this->submitForm([], 'Save');
    }

    // Create an entity with datetime field.
    $this->drupalGet('entity_test/add');
    $value = time() + 24 * 60 * 60;
    $date = DrupalDateTime::createFromTimestamp($value);
    $date_format = DateFormat::load('html_date')->getPattern();
    $time_format = DateFormat::load('html_time')->getPattern();
    $entityEdit = [
      $this->fieldName . '[0][value][date]' => $date->format($date_format),
      $this->fieldName . '[0][value][time]' => $date->format($time_format),
    ];
    $this->submitForm($entityEdit, 'Save');

    // Make sure entity was created.
    preg_match('|entity_test/manage/(\d+)|', $this->getUrl(), $match);
    $id = $match[1];
    $this->assertSession()->pageTextContains('entity_test ' . $id . ' has been created.');

    // Make sure external assets for formatter are loaded to the page.
    $this->drupalGet('entity_test/' . $id);
    foreach ($assets as $asset) {
      $this->assertSession()
        ->responseContains($asset);
    }

    // Enable usage of local assets.
    $this->drupalGet('admin/config/development/field-timer');
    $this->submitForm(['asset_source' => 'local'], 'Save configuration');

    // Make sure external assets for formatter are not loaded to the page.
    $this->drupalGet('entity_test/' . $id);
    foreach ($assets as $asset) {
      $this->assertSession()
        ->responseNotContains($asset);
    }
  }

  public function assetSourcesDataProvider() {
    yield [
      'field_timer_county',
      [],
      [
        'https://cdn.jsdelivr.net/gh/brilsergei/county@0.0.1/js/county.js',
        'https://cdn.jsdelivr.net/gh/brilsergei/county@0.0.1/css/county.css',
      ],
    ];

    yield [
      'field_timer_countdown',
      ['regional' => 'sr'],
      [
        'https://cdn.jsdelivr.net/gh/kbwood/countdown@2.1.0/dist/js/jquery.plugin.min.js',
        'https://cdn.jsdelivr.net/gh/kbwood/countdown@2.1.0/dist/js/jquery.countdown.min.js',
        'https://cdn.jsdelivr.net/gh/kbwood/countdown@2.1.0/dist/css/jquery.countdown.css',
        'https://cdn.jsdelivr.net/gh/kbwood/countdown@2.1.0/dist/js/jquery.countdown-sr.js',
      ],
    ];

    yield [
      'field_timer_countdown_led',
      [],
      ['css/field_timer-js-delivr.css'],
    ];
  }

}
