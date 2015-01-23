Description
-----------
This module extends the functionality of the Islandora Webform module to
provide digital repository curators the ability to ingest data collected from
webform submissions. This data may be ingested into the islandora object that
is the subject of the webform submission. Or it may be used to create a new
object which is related to the original object. Related objects can be displayed
in a block along with the object.


Requirements
------------
Islandora Webform module 7.x
(See Islandora Webform module for further requirements)


Installation and Configuration
------------------------------
1. See installation instructions for Islandora Webform module if not already
   installed and configured.

2. Login as an administrator. Enable the Islandora Webform Ingest module at
   "Administer" -> "Modules".

3. Go to Islandora Settings for a webform. In addition to the settings provided
   by the Islandora Webform module, Islandora Ingest settings are added. These
   permit you to enable ingest, and to configure other ingest settings: the
   destination for the ingested data (existing object or new object); in the
   case of new object, its content model, relationship to the existing object,
   and namespace.
   NOTE that if you will be ingesting to the current object, a content model must
   be selected in the Islandora Options section above.
   NOTE also that any changes in the destination content model will cause any
   previously saved ingest mappings for this form (see next) to be lost.

4. For each webform component that you wish to serve as a source of ingested
   data, edit the component and expand the "Islandora Ingest Mapping" section of
   the form. Under "Ingest?", select either "Append" or "Replace" (see detailed
   explanation below). Then you can select from the list of compatible
   destination datastreams for this component. If the datastream uses a form
   (e.g. MODS), select the field on that form that you wish to ingest to.
   "File" components can only be mapped to binary datastreams with compatible mime
   types.

5. If you are ingesting to new objects, go to "Administer" -> "Structure"
   -> "Blocks" and configure the block corresponding to the relationship you
   defined above (in step #3). In addition to setting which region on the page
   the related blocks should appear in, you can set the block title, the format
   of the  objects (grid or list with thumbnails, or links), how they should be
   sorted, and how many should appear in the block.

Ingesting Webform Submissions
-----------------------------


Behavior of "Append" and "Replace"
----------------------------------



Upgrading from previous versions
--------------------------------

1. MAKE A DATABASE BACKUP. Upgrading this module may entail a number of database
   changes. If you encounter an error and need to downgrade, you must
   restore the previous database. You can make a database backup with your
   hosting provider, using the Backup and Migrate module, or from the command
   line.

2. Copy the entire islandora_webform directory the Drupal modules directory, replacing
   the old copy. DO NOT KEEP THE OLD COPY in the same directory or
   anywhere Drupal could possibily find it. Delete it from the server.

3. Login as an administrative user or change the $update_free_access in
   update.php to TRUE.

4. Run update.php (at http://www.example.com/update.php).

Support
-------
TODO