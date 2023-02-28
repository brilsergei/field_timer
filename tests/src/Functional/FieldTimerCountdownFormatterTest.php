<?php

namespace Drupal\Tests\field_timer\Functional;

use Drupal\field_timer\Plugin\Field\FieldFormatter\FieldTimerCountdownFormatter;
use Drupal\Tests\datetime\Functional\DateTimeTimeAgoFormatterTest;

class FieldTimerCountdownFormatterTest extends DateTimeTimeAgoFormatterTest {

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
      'fields[' . $this->fieldName . '][type]' => 'field_timer_countdown',
    ];
    $this->submitForm($edit, 'Save');

    $this->submitForm([], $this->fieldName . '_settings_edit');
    $type = FieldTimerCountdownFormatter::TYPE_COUNTDOWN;
    $prefix = 'fields[' . $this->fieldName . '][settings_edit_form][settings]';
    $edit = [
      $prefix . '[type]' => $type,
      $prefix . '[use_system_language]' => TRUE,
      $prefix . '[format]' => 'M j, Y H:i:s',
      $prefix . '[layout]' => '<b>{d<}{dn} {dl} and {d>}</b>',
      $prefix . '[compact]' => TRUE,
      $prefix . '[significant]' => 3,
      $prefix . '[timeSeparator]' => '::',
      $prefix . '[padZeroes]' => TRUE,
    ];
    $types = $this->getOptions($prefix . '[type]');
    $this->submitForm($edit, 'Update');
    $this->submitForm([], 'Save');

    $this->assertSession()->pageTextContains('Type: ' . $types[$type]);
    $this->assertSession()->pageTextContains('Use system language: Yes');
    $this->assertSession()->pageTextContains('Format: M j, Y H:i:s');
    $this->assertSession()->pageTextContains('Layout: <b>{d<}{dn} {dl} and {d>}</b>');
    $this->assertSession()->pageTextContains('Compact: Yes');
    $this->assertSession()->pageTextContains('Granularity: 3');
    $this->assertSession()->pageTextContains('Time separator: ::');
    $this->assertSession()->pageTextContains('Pad with zeroes: Yes');
  }

}
