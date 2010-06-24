; $Id$

The scripts to convert punBB forum to Drupal.

Dependencies
--

You need enable these modules before install this module:
- forum
- comments
- taxonomy
- bbcode

1. Edit $F_PREFIX variable on f.module to match your punBB settings.
2. Run /forum-migrate/users to import users.
3. Run /forum-migrate/forum-containers to import forum containers.
4. Run /forum-migrate/forums to import forums.
5. Run /forum-migrate/forum-topics to import forum-topics.
6. Run /forum-migrate/forum-comments to import comments.
7. Run /forum-migrate/302-generate to get the redirect code.

You can uninstall this module (and bbcode module) when you completed the convertation.
