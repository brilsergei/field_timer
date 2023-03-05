<?php

namespace Drupal\Tests\field_timer\Unit;

use Drupal\field_timer\LibraryAssetLinks;
use Drupal\Tests\UnitTestCase;

class LibraryAssetLinksTest extends UnitTestCase {

  /**
   * @dataProvider replaceLocalWithJsDelivrDataProvider
   */
  public function testReplaceLocalWithJsDelivr(array $libraries, array $expected) {
    $libraryAssetLinks = new LibraryAssetLinks();
    $result = $libraryAssetLinks->replaceLocalWithJsDelivr($libraries);
    $this->assertEquals($expected, $result);
  }

  public function replaceLocalWithJsDelivrDataProvider() {
    $libraries = [
      'init' => [
        'css' => ['component' => ['css/field_timer-local.css' => []]],
      ],
      'county' => [
        'js' => ['/libraries/county/js/county.js' => []],
        'css' => ['component' => ['/libraries/county/css/county.css' => []]],
      ],
      'jquery.countdown' => [
        'js' => [
          '/libraries/jquery.countdown/js/jquery.plugin.min.js' => [],
          '/libraries/jquery.countdown/js/jquery.countdown.min.js' => [],
        ],
        'css' => [
          'component' => [
            '/libraries/jquery.countdown/css/jquery.countdown.css' => [],
          ],
        ],
      ],
      'jquery.countdown.hy' => [
        'js' => ['/libraries/jquery.countdown/js/jquery.countdown-hy.js' => []],
      ],
    ];

    $expected = [
      'init' => [
        'css' => ['component' => ['css/field_timer-js-delivr.css' => []]],
      ],
      'county' => [
        'js' => [
          'https://cdn.jsdelivr.net/gh/brilsergei/county@0.0.1/js/county.js' => [
            'type' => 'external',
          ],
        ],
        'css' => [
          'component' => [
            'https://cdn.jsdelivr.net/gh/brilsergei/county@0.0.1/css/county.css' => [
              'type' => 'external',
            ],
          ],
        ],
      ],
      'jquery.countdown' => [
        'js' => [
          'https://cdn.jsdelivr.net/gh/kbwood/countdown@2.1.0/dist/js/jquery.plugin.min.js' => [
            'type' => 'external',
          ],
          'https://cdn.jsdelivr.net/gh/kbwood/countdown@2.1.0/dist/js/jquery.countdown.min.js' => [
            'type' => 'external',
          ],
        ],
        'css' => [
          'component' => [
            'https://cdn.jsdelivr.net/gh/kbwood/countdown@2.1.0/dist/css/jquery.countdown.css' => [
              'type' => 'external',
            ],
          ],
        ],
      ],
      'jquery.countdown.hy' => [
        'js' => [
          'https://cdn.jsdelivr.net/gh/kbwood/countdown@2.1.0/dist/js/jquery.countdown-hy.js' => [
            'type' => 'external',
          ],
        ],
      ],
    ];

    yield [$libraries, $expected];
  }

}
