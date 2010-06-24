<?php
// $Id$

/**
 * Find forum vocabulary.
 */
function punbb2drupal_forum_vocab_id() {
  static $vid;
  
  if (!$vid) {
    $vid = db_result(db_query("SELECT vid FROM {vocabulary} WHERE module = 'forum'"));
  }

  return $vid;
}

/**
 * Callback for containers importing.
 */
function punbb2drupal_import_containers() {
  global $F_PREFIX;
  
  $counter = db_result(db_query("SELECT COUNT(*) FROM {punbb2drupal_container} WHERE 1"));
  if (!$counter) {
    $sql = "SELECT * FROM {$F_PREFIX}_categories WHERE 1 ORDER BY disp_position";
    $q = db_query($sql);
    while ($v = db_fetch_object($q)) {
      $term = (array)punbb2drupal_container_convert($v);
      taxonomy_save_term($term);
      drupal_set_message("Imported container <strong>{$term['name']}</strong>.");
    }
    
    return '';
  }
  
  return '<div class="error">The script was already run.</div>';
}

/**
 * Convert punBB group to drupal-term structure.
 */
function punbb2drupal_container_convert($v) {
  $record = new stdClass;
  $record->vid = punbb2drupal_forum_vocab_id();
  $record->name = $v->cat_name;
  $record->description = '';
  $record->weight = $v->disp_position;

  $record->type = 'container';
  $record->id = $v->id;

  return $record;
}

/**
 * Callback for forum importing.
 */
function punbb2drupal_import_forums() {
  global $F_PREFIX;

  // Container importing script must be run before this script
  $counter = db_result(db_query("SELECT COUNT(*) FROM {punbb2drupal_container} WHERE 1"));
  if ($counter) {
    $counter = db_result(db_query("SELECT COUNT(*) FROM {punbb2drupal_forum} WHERE 1"));
    if (!$counter) {
      $sql = "SELECT * FROM {$F_PREFIX}_forums WHERE 1 ORDER BY disp_position";
      $q = db_query($sql);
      while ($v = db_fetch_object($q)) {
        $term = (array)punbb2drupal_forum_convert($v);
        taxonomy_save_term($term);
        drupal_set_message("Imported forum <em>{$term['name']}</em>.");
      }

      return '';
    }

    return '<div class="error">The script was already run.</div>';
  }
  
  return '<div class="error">The container importing script shoud be run before this script.</div>';
}

function punbb2drupal_forum_convert($v) {
  $record = new stdClass;
  $record->name = $v->forum_name;
  $record->description = $v->forum_desc;
  $record->parent[] = db_result(db_query("SELECT tid FROM {punbb2drupal_container} WHERE id = %d", $v->cat_id));  
  $record->weight = $v->disp_position;
  $record->vid = punbb2drupal_forum_vocab_id();
  $record->id = $v->id;
  $record->data['moderators'] = unserialize($v->moderators);

  return $record;
}

/**
 * Save term extra function.
 */
function punbb2drupal_taxonomy_insert($edit) {
  if (!empty($edit['id'])) {
    $record = new stdClass();
    $record->tid = $edit['tid'];
    $record->id  = $edit['id'];
    
    if ($edit['type'] === 'container') {
      $containerTIDs = variable_get('forum_containers', array());
      $containerTIDs[] = $edit['tid'];
      
      variable_set('forum_containers', $containerTIDs);
      
      drupal_write_record('punbb2drupal_container', $record);
    }
    else {
      $record->data = serialize($edit['data']);
      drupal_write_record('punbb2drupal_forum', $record);
    }
  }
}
