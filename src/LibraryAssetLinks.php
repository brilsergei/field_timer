<?php

namespace Drupal\field_timer;

/**
 * Helper which replaces asset links in library definitions.
 *
 * Asset links are replaced from local paths to remote paths.
 */
class LibraryAssetLinks {

  /**
   * Replaces local asset links with links to jsDelivr.
   *
   * @param array $libraries
   *
   * @return array
   */
  public function replaceLocalWithJsDelivr(array $libraries) {
    // Use CSS which contains link to images in CDN.
    unset($libraries['init']['css']['component']['css/field_timer-local.css']);
    $libraries['init']['css']['component']['css/field_timer-js-delivr.css'] = [];

    // Replace links to local assets with links to assets in CDN.
    foreach ($libraries['county']['js'] as $js => $data) {
      $this->replaceLocalCountyWithJsDelivr($libraries['county']['js'], $js);
    }
    foreach ($libraries['county']['css']['component'] as $css => $data) {
      $this->replaceLocalCountyWithJsDelivr($libraries['county']['css']['component'], $css);
    }
    foreach ($libraries['jquery.countdown']['js'] as $js => $data) {
      $this->replaceLocalJqueryCountdownWithJsDelivr($libraries['jquery.countdown']['js'], $js);
    }
    foreach ($libraries['jquery.countdown']['css']['component'] as $css => $data) {
      $this->replaceLocalJqueryCountdownWithJsDelivr($libraries['jquery.countdown']['css']['component'], $css);
    }
    foreach ($libraries as $name => $library) {
      if (str_contains($name, 'jquery.countdown.')) {
        foreach ($libraries[$name]['js'] as $js => $data) {
          $this->replaceLocalJqueryCountdownWithJsDelivr($libraries[$name]['js'], $js);
        }
      }
    }

    return $libraries;
  }

  /**
   * Replaces local asset links of County with links to jsDelivr.
   *
   * @param array $assetArray
   * @param string $oldLink
   *
   * @return void
   */
  protected function replaceLocalCountyWithJsDelivr(array &$assetArray, string $oldLink) {
    $this->replaceLocalLinkWithJsDelivr($assetArray, $oldLink, '/libraries/county', 'https://cdn.jsdelivr.net/gh/brilsergei/county@0.0.1');
  }

  /**
   * Replaces local asset links of Countdown with links to jsDelivr.
   *
   * @param array $assetArray
   * @param string $oldLink
   *
   * @return void
   */
  protected function replaceLocalJqueryCountdownWithJsDelivr(array &$assetArray, string $oldLink) {
    $this->replaceLocalLinkWithJsDelivr($assetArray, $oldLink, '/libraries/jquery.countdown', 'https://cdn.jsdelivr.net/gh/kbwood/countdown@2.1.0/dist');
  }

  /**
   * Replaces a local assets link with corresponding link to jsDelivr.
   *
   * @param array $assetArray
   * @param string $oldLink
   * @param string $search
   * @param string $replace
   *
   * @return void
   */
  protected function replaceLocalLinkWithJsDelivr(array &$assetArray, string $oldLink, string $search, string $replace) {
    $data = $assetArray[$oldLink];
    unset($assetArray[$oldLink]);
    $newLink = str_replace($search, $replace, $oldLink);
    $data['type'] = 'external';
    $assetArray[$newLink] = $data;
  }

}
