<?php
// $Id$

/**
 * Callback for importing the comments.
 */
function punbb2drupal_import_comments() {
  global $F_PREFIX;

  $counter = db_result(db_query(
    "SELECT COUNT(*) FROM {punbb2drupal_comment} WHERE 1"
  ));

  if (!$counter) {
    module_load_include('inc', 'bbcode', 'bbcode-filter');

    $q = db_query(
      "SELECT p.*, t.id AS punbb2drupal_topic_id FROM {$F_PREFIX}_posts p"
        . " LEFT JOIN {$F_PREFIX}_topics t ON p.id = t.first_post_id"
        . " WHERE 1"
        . " ORDER BY id ASC"
    );

    while ($v = db_fetch_object($q)) {
      // The first post of thread was already imported
      if (!$v->punbb2drupal_topic_id) {
        $comment = (array)punbb2drupal_comment_convert($v);
        
        // Save extra values.
        $record = new stdClass;
        $comment['cid'];
        $record->id   = $v->id;
        $record->data = serialize($comment['data']);
        $record->cid  = comment_save($comment);

        drupal_write_record('punbb2drupal_comment', $record);

        drupal_set_message("Imported comment by {$v->poster} on " . format_date($v->posted));
      }
    }
    
    return '';
  }

  return '<div class="error">Script was already run.</div>';
}

function punbb2drupal_comment_convert($v) {
  $comment = new stdClass;
  $comment->author    = $comment->name = $v->poster;
  $comment->format    = 2;
  $comment->comment   = _bbcode_filter_process($v->message, $comment->format);
  $comment->subject   = truncate_utf8(drupal_html_to_text($comment->comment), 128, TRUE);
  $comment->nid       = db_result(db_query("SELECT nid FROM {punbb2drupal_topic} WHERE id = %d", $v->topic_id));
  $comment->timestamp = $v->posted;
  $comment->data      = $v;
  $comment->uid       = db_result(db_query("SELECT uid FROM {punbb2drupal_user} WHERE id = %d", $v->poster_id));
  return $comment;
}
