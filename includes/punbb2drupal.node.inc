<?php
// $Id$

/**
 * Callback for topics importing.
 */
function punbb2drupal_import_topics() {
  global $F_PREFIX;

  $counter = db_result(db_query("SELECT COUNT(*) FROM {punbb2drupal_topic} WHERE 1"));
  if (!$counter) {
    module_load_include('inc', 'bbcode', 'bbcode-filter');

    $q = db_query(
      "SELECT * FROM {$F_PREFIX}_topics"
        . " WHERE 1"
        . " ORDER BY id ASC"
    );

    while ($v = db_fetch_object($q)) {
      $post = db_fetch_object(db_query(
        "SELECT * FROM {$F_PREFIX}_posts WHERE id = %d", $v->first_post_id
      ));
      
      $node = punbb2drupal_topic_convert($v, $post);
      node_save($node);
      
      drupal_set_message("Imported <em>{$node->title}</em> forum topic.");
    }

    return '';
  }

  return '<div class="error">Script was already run.</div>';
}

function punbb2drupal_topic_convert($topic, $post) {
  $node = new stdClass;
  $node->type    = 'forum';
  $node->title   = $topic->subject;
  $node->created = $topic->posted;
  $node->status  = 1;
  $node->comment = $topic->closed ? 1 : 2;
  $node->uid     = db_result(db_query(
    "SELECT uid FROM {punbb2drupal_user} WHERE id = %d", $post->poster_id
  ));

  /**
   * Body and teaser
   */
  // BBCode
  $node->format = 2; // Full HTML input format.
  
  $post->message = _bbcode_filter_process($post->message, $node->format);
  $node->body = $node->teaser = $post->message;

  /**
   * Taxonomy
   */
  // Find Term ID
  $termID = db_result(db_query(
    "SELECT tid FROM {punbb2drupal_forum} WHERE id = %d", 
    $topic->forum_id
  ));
  
  // Find vocab ID
  $vocabID = db_result(db_query("SELECT vid FROM {term_data} WHERE tid = %d", $termID));
  $node->taxonomy[$vocabID][] = $termID;
  $node->vid = $vocabID;
  $node->tid = $termID;

  /**
   * Extra values.
   */
  $node->data = array(
    'topic' => $topic,
    'post'  => $post,
  );
  
  return $node;
}

/**
 * Save values on forum-node created.
 */
function punbb2drupal_node_api_insert($node) {
  if (!empty($node->data)) {
    $record = new stdClass;
    $record->nid  = $node->nid;
    $record->id   = $node->data['topic']->id;
    $record->data = serialize($node->data);

    drupal_write_record('punbb2drupal_topic', $record);
  }
}
