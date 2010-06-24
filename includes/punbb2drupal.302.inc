<?php
// $Id$

/**
 * Simple form with a textarea to display redirect command for Apacher user.
 */
function punbb2drupal_302_form(&$form_state) {
  $form[] = array(
    '#type'  => 'textarea',
    '#title' => 'Append these lines to your .htaccess',
    '#value' => punbb2drupal_302_generate(),
    '#rows' => 30,
  );

  return $form;
}

function punbb2drupal_302_generate() {
  $items = array();
  
  $q = db_query("SELECT tid, id FROM {punbb2drupal_forum} ORDER BY id ASC");
  while ($v = db_fetch_object($q)) {
    $items[] = "Redirect 302 /viewforum.php?id={$v->id} " . url("taxonomy/term/{$v->tid}", array('absolute' => TRUE));
  }
  
  $q = db_query("SELECT nid, id FROM {punbb2drupal_topic} ORDER BY id ASC");
  while ($v = db_fetch_object($q)) {
    $items[] = "Redirect 302 /viewtopic.php?id={$v->id} " . url("node/{$v->nid}", array('absolute' => TRUE));
  }

  return implode("\n", $items);
}
