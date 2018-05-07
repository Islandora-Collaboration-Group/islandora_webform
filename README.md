# The Islandora Webform module
(last updated: Jun 15, 2017, Peter MacDonald)

## Table of Contents

1. **[Overview: Islandora Webform](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/README.md)**
2. [How to Install the Islandora Webform module](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_installation.md)
3. [Summary of Steps for Creating an Islandora Webform](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_steps.md)
4. [How to Create a Webform using the Islandora Webform module](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_creation.md)
5. [For End Users: How to Use an Islandora Webform](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_for_users.md)
6. [Use Case: Islandora Webform for Transcriptions (DHi@Hamilton College)](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_transcriptions.md)

***

## Description

* The Islandora Webform (IW) module uses the capabilities of the standard Drupal Webform module to enable users to submit comments (captions, tags, transcriptions, etc.) on digital objects in an Islandora repository.
* In Islandora, a link to the webform appears on the page of qualifying repository objects. When clicked by an authorized visitor, the link launches the webform that gathers data from the user and puts the submitted data into a queue waiting for approval by a webform manager.
* When the submission is approved, the form values are ingested either 1) into the MODS (or another specific datastream) of the Fedora object being commented on, or 2) into a completely new Fedora object. If 2) is used, a relationship statement is placed in the RELS-EXT datastream connecting the submission Fedora object to the original Fedora object.
* All the submissions for a specific object can then be displayed along with the original object using a dedicated Drupal block.
* The Islandora Webform Ingest module, which is provided with this module and can be enabled separately, provides digital repository curators the ability to directly ingest webform submissions into a datastream of the associated object.

***

## Screenshots

**An Islandora objectpage showing a link to the Islandora webform at the bottom of the window:**

![webform_12.png](/docs/images/webform_12.png)

***

**View of a webform as seen by an end user:**

![webform_13.png](/docs/images/webform_13.png)

***

**View of what users can see if they want to view, edit or delete their submission:**

![webform_14.png](/docs/images/webform_14.png)

***

## Requirements

* Drupal 7.x
* Webform 7.4.x
* Islandora 7.x with Islandora Collections and at least one Solution Pack enabled.

## Credits

* The Islandora Webform module was conceived by the Islandora Collaboration Group (ICG)
  * https://github.com/Islandora-Collaboration-Group
* The module development was coordinated by the Digital Humanities Initiative (DHi) at Hamilton College (http://dhinitiative.org/) and supported by funds from an Andrew W. Mellon Foundation grant to DHi.
* The development was performed by Common Media (Patrick Dunlavey, developer)
  * https://commonmedia.com/
* Beta testing was first performed by the DHi Collection Development Team and reviewed by other members of the ICG.

## How to Participate

* Send a request to the ICG (islandora-consortium-group@googlegroups.com)
