<?php

namespace Drupal\Tests\field_timer\Functional\Update;

use Drupal\FunctionalTests\Update\UpdatePathTestBase;

/**
 * Tests field_timer_update_10201.
 *
 * @group field_timer
 */
class InitialConfigUpdate extends UpdatePathTestBase {

  /**
   * @inheritDoc
   */
  protected function setDatabaseDumpFiles() {
    $this->databaseDumpFiles = [
      __DIR__ . '/../../../fixtures/update/drupal-10.0.2.bare.standard.field_timer-2.0.1.php.gz',
    ];
  }

  /**
   * Tests creation of config on update.
   */
  public function testConfigCreation() {
    $config = $this->config('field_timer.config');
    $this->assertEmpty($config->get('asset_source'));

    // Run updates.
    $this->runUpdates();

    $config = $this->config('field_timer.config');
    $this->assertSame('local', $config->get('asset_source'));
  }

}
