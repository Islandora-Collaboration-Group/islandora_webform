# Summary of Steps for Creating an Islandora Webform
(last updated: May 23, 2017, Peter MacDonald)

This Summary of Steps skips giving you the actual values to put in the settings -- it just says which settings I use for webform for DHi at Hamilton. YMMV. For full details see the page Islandora Webform module. (Peter)

## Table of Contents

1. [Overview: Islandora Webform](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/README.md)
2. [How to Install the Islandora Webform module](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_installation.md)
3. **[Summary of Steps for Creating an Islandora Webform](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_steps.md)**  
    1. Create the New Webform  
    2. Add Components  
    3. Configure Islandora Settings  
    4. Configure Ingest Mapping for each Component  
    5. Configure Confirmation Page  
4. [How to Create a Webform using the Islandora Webform module](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_creation.md)
5. [For End Users: How to Use an Islandora Webform](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_for_users.md)
6. [Use Case: Islandora Webform for Transcriptions (DHi@Hamilton College)](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_transcriptions.md)

***

## 1. Create the New Webform
  * Content > Add content > Webform
    * Fill out "Create Webform" page
      * Title, Publishing options
    * Save

## 2. Add Components
  * Content > Webforms > find your webform > Components
    * Create components: Label, Type, Required
    * Add
    * Fill in "Edit component" page
      * Description: (optional, I leave it blank)
      * Display: Placeholder (used sometimes), Disabled (used sometimes)
      * Save Component
    * Repeat until all your components are created.
    * Save (again)

## 3. Configure Islandora Settings
  * Content > Webforms > find your webform > Components > Islandora settings
    * Islandora Options: Enable, Content model filter, Collection filter, PID search string, Add a link to..., Link text, Link help text
    * Islandora Ingest: Enabled, Ingest destination, Relationship to current object, Namespace of new object
    * Save configuration

## 4. Configure Ingest Mapping for each Component
  * Content > Webforms > find your webform > Components
    * Select a component > Edit
      * Islandora Ingest Mapping: Ingest?, DataStream, Field
      * Save component

## 5. Configure Confirmation Page
  * Content > Webforms > find your webform > Components > Form settings
    * Submission Settings: Confirmation message, Text format, No redirect
    * Inline Islandora Webform: AJAX
    * Submission Access: [select roles]
    * Save configuration
