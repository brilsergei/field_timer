<?php

/**
 * Allow modules to define date properties of
 * an entity type.
 * 
 * @return array which keys are entity types and values
 * are arrays described properties. Each key of subarray is
 * property name and value is property human readable name.
 * 
 * @see function field_timer_field_timer_entity_properties
 */
function hook_field_timer_entity_properties() {
  $properties = array (
      'node' => array (
          'created' => t('Node create date'),
          'changed' => t('Node last change date')
      ),
  );
  return $properties;
}

/**
 * Allow module to alter description of entity date properties.
 * 
 * @see function hook_field_timer_entity_properties
 */
function hook_field_timer_entity_properties_alter(&$properties) {
  
}