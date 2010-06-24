<?php
// $Id$

/**
 * Callback for user importing.
 */
function punbb2drupal_import_users() {
  global $F_PREFIX;

  $counter = db_result(db_query("SELECT COUNT(*) FROM {punbb2drupal_user} WHERE 1"));
  if (empty($counter)) {
    $sql = "SELECT * FROM {$F_PREFIX}_users WHERE id > 1 ORDER BY id";
    $q = db_query($sql);
    while ($v = db_fetch_object($q)) {
      $account = (array)fPunbbUser2DrupalUser($v);

      // Find for existing user on system
      $sql = "SELECT uid FROM {users} WHERE name LIKE '%s'";
      $uid = db_result(db_query($sql, $account['name']));
      if ($uid) {
        $record = new stdClass;
        $record->uid = $uid;
        $record->id  = $account['id'];
        $record->data = serialize($account);
        drupal_write_record('punbb2drupal_user', $record);
        
        drupal_set_message("Imported {$account['name']} ({$account['mail']})");
      }
      else {
        $account = user_save('', $account);
        drupal_set_message("Imported {$account->name} ({$account->mail})");
      }
    }

    return '&nbsp;';
  }

  return '<div class="error">Script was already run.</div>';
}

/**
 * Convert punBB user object to Drupal.
 */
function fPunbbUser2DrupalUser($v) {
  $user = new stdClass;
  $user->punbb2drupal_module = TRUE;

  foreach ($v as $key => $value) {
    switch ($key) {
      case 'username':
        $user->name = $v->username;
        break;

      case 'password':
        $user->pass = $user->password = $v->password;
        break;

      case 'email':
        $user->mail = $user->init = $v->email;
        break;
      
      case 'registered':
        $user->created = $v->registered;
        break;
      
      case 'last_visit':
        $user->login = $user->access = $v->last_visit;
        break;
      
      case 'timezone':
        $user->timezone = 3600 * $v->timezone;
        break;

      default:
        $user->$key = $v->{$key};
    }
  }
  
  // Setup role
  $fPunbbGroups2Drupal = array(
    1 => 3, // 'Admin',
    // 2 => 1, // 'Guest',
    // 3 => 2, // 'Members',
    4 => 8, // 'Moderators',
  );
  
  if (isset($fPunbbGroups2Drupal[$v->group_id])) {
    $rid = $fPunbbGroups2Drupal[$v->group_id];
    $user->roles[$rid] = $rid;
  }

  return $user;
}

/**
 * Helper function to save extra function from
 */
function punbb2drupal_user_insert(&$edit, &$account = NULL) {
  if (!empty($edit['punbb2drupal_module'])) {
    $fields = array('uid', 'id', 'password', 'salt');
    $record = new stdClass;
    foreach ($fields as $k) {
      $record->{$k} = $edit[$k];
    }

    $record->data = serialize($edit);
    drupal_write_record('punbb2drupal_user', $record);
  }
}
