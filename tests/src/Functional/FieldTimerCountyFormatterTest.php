<?php

namespace Drupal\Tests\field_timer\Functional;

use Drupal\field_timer\Plugin\Field\FieldFormatter\FieldTimerCountyFormatter;
use Drupal\Tests\datetime\Functional\DateTimeTimeAgoFormatterTest;

class FieldTimerCountyFormatterTest extends DateTimeTimeAgoFormatterTest {

  protected static $modules = ['field_timer', 'entity_test', 'field_ui'];

  /**
   * @var string
   * @see \Drupal\Tests\datetime\Functional\DateTimeTimeAgoFormatterTest::setUp
   */
  private string $fieldName = 'field_datetime';

  /**
   * Tests the formatter settings.
   */
  public function testSettings() {
    $this->drupalGet('entity_test/structure/entity_test/display');

    $edit = [
      'fields[' . $this->fieldName . '][region]' => 'content',
      'fields[' . $this->fieldName . '][type]' => 'field_timer_county',
    ];
    $this->submitForm($edit, 'Save');

    $this->submitForm([], $this->fieldName . '_settings_edit');
    $animation = FieldTimerCountyFormatter::ANIMATION_SCROLL;
    $theme = FieldTimerCountyFormatter::COUNTY_THEME_GRAY;
    $prefix = 'fields[' . $this->fieldName . '][settings_edit_form][settings]';
    $edit = [
      $prefix . '[animation]' => $animation,
      $prefix . '[speed]' => 1000,
      $prefix . '[theme]' => $theme,
      $prefix . '[background]' => '#bababa',
      $prefix . '[reflection]' => 0,
    ];
    $animations = $this->getOptions($prefix . '[animation]');
    $themes = $this->getOptions($prefix . '[theme]');
    $this->submitForm($edit, 'Update');
    $this->submitForm([], 'Save');

    $this->assertSession()->pageTextContains('Animation: ' . $animations[$animation]);
    $this->assertSession()->pageTextContains('Speed: ' . 1000);
    $this->assertSession()->pageTextContains('Theme: ' . $themes[$theme]);
    $this->assertSession()->pageTextContains('Background: #bababa');
    $this->assertSession()->pageTextContains('Reflection: Disabled');
  }

}
