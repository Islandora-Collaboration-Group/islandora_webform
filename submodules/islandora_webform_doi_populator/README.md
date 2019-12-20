Description
-----------
This module adds a new webform component field to pre'populate DOI information and prepare
it for ingestion


Requirements
------------
Islandora Webform module 
Islandora Webform Ingest


Installation and Configuration
------------------------------
1. See installation instructions for Islandora Webform module if not already
   installed and configured.

2. Login as an administrator. Enable the Islandora Webform DOI Populator module at
   "Administer" -> "Modules".
   
3. Only one DOI field can be added and will be recognized per form, field has to have
doi as the machine name (current default)


Permissions
-----------------------------
This module defines the following permissions:
1. Ingest Islandora Webform Submissions - enable for roles that should be able
   to ingest webform submissions.


Ingesting Webform Submissions
-----------------------------
1. Once you have received some webform submissions, you will see them listed under the
   "Submissions" tab for the object that the submissions were in reference to. At the
   right side of this table, an "Ingest" column has been added, and if a submission
   can be ingested, an "Ingest" link will appear there.

2. Clicking on the "Ingest" link will bring up a page that displays the datastreams that
   were configured as targets for webform values, with those values entered. If
   a destination datastream provides an edit form (e.g. MODS), then that form will
   be displayed, and you can edit the values shown there. Once you are satisfied with
   the content and metadata, you can click "Ingest this Submission". This will cause
   the data to be ingested, either to the original object, or to a new object. If
   ingested to a new object, it will be created with a rels-ext relation to the
   original object.

Upgrading from previous versions
--------------------------------

1. MAKE A DATABASE BACKUP. Upgrading this module may entail a number of database
   changes. If you encounter an error and need to downgrade, you must
   restore the previous database. You can make a database backup with your
   hosting provider, using the Backup and Migrate module, or from the command
   line.

2. Copy the entire islandora_webform_ingest directory the Drupal modules directory, replacing
   the old copy. DO NOT KEEP THE OLD COPY in the same directory or
   anywhere Drupal could possibily find it. Delete it from the server.

3. Login as an administrative user or change the $update_free_access in
   update.php to TRUE.

4. Run update.php (at http://www.example.com/update.php).

Support
-------
TODO
