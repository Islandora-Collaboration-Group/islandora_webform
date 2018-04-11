# How to Install the Islandora Webform module
(last updated: May 26, 2017, Peter MacDonald)

Guide for use: If you are an Islandora administrator with previous experience setting up Drupal webforms, then you will find this document offers more details than you probably need, but we hope that even you will find the level of detail into which this documentation goes helps you avoid making mistakes that can result in frustration and delays in implementation.

## Table of Contents

1. [Overview: Islandora Webform](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/README.md)
2. **[How to Install the Islandora Webform module](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_installation.md)**  
    1. Installation preliminaries
    2. Installation summary
    3. Install the Drupal "Webform" module
    4. Configuration
    5. Install the "Webform AJAX" module
    6. Install the "Islandora Webform" module
    7. Create Drupal Roles and set permissions for users of the Islandora Webform module
    8. Create Drupal User Accounts and assign Roles for users of the Islandora Webform module
    9. Enable the Drupal block for Islandora Webform submissions
    10. APPENDIX  
        * SOLR/GSearch Configuration  
        * Enabling and configuring Islandora Webform Ingest  
        * Upgrading from an earlier version of the Islandora Webform module  
3. [Summary of Steps for Creating an Islandora Webform](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_steps.md)
4. [How to Create a Webform using the Islandora Webform module](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_creation.md)
5. [For End Users: How to Use an Islandora Webform](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_for_users.md)
6. [Use Case: Islandora Webform for Transcriptions (DHi@Hamilton College)](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_transcriptions.md)

***

## 1. Installation preliminaries

* DHi last tested the IW module in February of 2017 using the following versions of each of these modules and libraries:

| Module Name                | Version       |
| -------------------------- |:-------------:|
| CentOs                     | -             |
| Drupal                     | 7.51          |
| Islandora                  | 7x-1.7        |
| Tuque                      | 7x-1.7        |
| Islandora Basic Collection | 7x-1.7        |
| Islandora XML Forms        | 7x-dev        |
| Webform                    | 7.x-4.14      |
| Webform AJAX               | 7.x-1.1       |

* Ensure that basic Islandora modules and dependencies are installed and working, namely
  * Islandora, Islandora Basic Colelction, Islandora XML Form, and Tuque.
  * Ensure that the Islandora connection to Fedora is working (administration > Islandora)

***

## 2. Installation summary

1. Copy the entire islandora_webform directory into the Drupal sites/all/modules directory (or subdirectory).
2. Login as an administrator. Enable the Islandora Webform module in the "Administer" -> "Modules"
3. Create a webform node at node/add/webform.
4. Configure Islandora settings for the webform to define which repository objects can show a link to this webform.
5. If manual webform linking is selected, go to each object that you want to link to a webform, and click the link to add it.

***

## 3. Install the Drupal "Webform" module

```
> drush dl -y webform
> drush en -y webform (add @sites for multi-site setups)
> drush -y updatedb (add @sites for multi-site setups)
```

## 4. Configuration

**Configure the Drupal "Webform" module**

Configuration is found at: Administer > Configuration > Content Authoring > Webform settings (or <your_site>/admin/config/content/webform)

***

![webform_18.png](/docs/images/webform_18.png)

Figure 1: Configuring the Drupal webform module (part 1 of 2)

***

* Uncheck any fields you don’t think you’ll need in any of your Webforms. [Or, just leave them all checked.]

***

![webform_15.png](/docs/images/webform_15.png)

Figure 2: Configuring the Drupal webform module (part 2 of 2)

***

* Fill in the e-mail values as appropriate.
* Click "Save configuration".

***

**Islandora Webform Configuration**

Configuration is found at: Islandora > Configuration > Islandora Webform (/admin/islandora/configure/iw).

1. "Webform link behavior": If not using inline webforms with webform_ajax, this controls whether clicking on a webform link opens a new page/tab or replaces the current window.
2. "Append webform links to bottom of object view?": Uncheck this if you want to override the normal positioning of the webform links below the object display.

***

## 5. Install the "Webform AJAX" module

```
> drush dl -y weborm_ajax
> drush en -y webform_ajax (add @sites if needed for all sites in a multi-site Drupal installation)
```

***

## 6. Install the "Islandora Webform" module

* The IW module actually consists of three modules and they all need to be enabled individually (in Drupal or with drush).
  * islandora_webform
  * islandora_webform_ingest
  * islandora_simple_text_solution_pack

* On the Drupal server, navigate to "sites/all/modules".
* [If you are upgrading the IW modules, see the APPENDIX to this document before proceeding.]
* Clone the islandora_webform module:
```
> git clone https://github.com/Islandora-Collaboration-Group/islandora_webform.git
```
* Enable the Islandora Webform module for each site that needs it.
```
> drush en -y islandora_webform
> drush en -y islandora_webform_ingest
> drush en -y islandora_example_simple_text
```
* Update the database.
```
> drush updatedb (or in the Drupal GUI: "<your_site>/update.php")
```
* The directory structure of the IW modules
```sites/all/modules
 |_islandora-webform
   |_islandora_webform_module
   |_submodules
     |_islandora_webform_ingest
       |_islandora_webform_ingest.module
       |_examples
         |_islandora_example_simple_text_solution_pack
           |_islandora_example_simple_text.module
```
* To learn more about each IW module, consult the README file for each one on the IW code distribution repo.
  * https://github.com/Islandora-Collaboration-Group/islandora_webform.git

**Ensure that all dependencies for these webform modules are installed and enabled**

* Administer > Modules
* Look for any Required but "missing" or "disabled" dependencies for the modules: Webform, Webform AJAX, Islandora Webform, and Islandora Webform Ingest modules, Islandora Example Simple Text.
* There are no global configuration options for the IW module. All configuration is managed separately for each webform created by users of the IW module.

***

## 7. Create Drupal Roles and set permissions for the Islandora Webform module

Islandora Webform permissions are found at: Modules > Islandora Webform > Permissions (/admin/people/permissions)

People permissions found at: Administer > People > Permissions > Roles (admin/people/permissions/roles)

1. Enable Islandora Webform for roles that should be able to add webform links to objects generally. Depending on your needs, you may need to create a couple of new Drupal user roles so you can give different sets of permissions to difference groups of webform users.
2. Link islandora objects to webforms - when manual webform linking is selected, enable for roles to add webform links on individual objects.

***

![webform_16.png](/docs/images/webform_16.png)

Figure 3: Configuring Drupal Roles for the Islandora Webform module users

***

* Add as many new roles as you think you'll need, e.g.
  * webform manager (=authenticated user + all webform permissions)
  * webform submitter (=authenticated user + limited webform permissions)
* Set desired permissions for each user role here:
  * Administer > Modules > Islandora Webform > Permissions (i.e., /admin/people/permissions#module-islandora_webform)

**Configure Islandora Webform Permissions**

* These permission are currently set restrictively, but can easily be loosened up.
  * Full management permissions are granted to the “administrator” and webform manager” roles.
  * Submission permissions are granted to the “webform submitter” role.
  * No webform permissions are granted to the “anonymous” and “authenticated” roles.

***

**Node (scroll down to the 'Webform:" settings)**

| Feature | anon. | authen. | admin. | webform mgr | webform submitter |
| ------------- |:-------------:| :-----:| :-------------: |:-------------:| :-----:|
| Webform: Create new content | - | - | X | X | X |
| Webform: Edit own content   | - | - | X | X | X |
| Webform: Edit any content   | - | - | X | X | - |
| Webform: Edit any content   | - | - | X | X | X |
| Webform: Delete any content | - | - | X | X | - |

***

**Islandora**

| Feature | anon. | authen. | admin. | webform mgr | webform submitter |
| ------------- |:-------------:| :-----:| :-------------: |:-------------:| :-----:|
| View repository objects | X | X | X | X | X |

***

**Islandora Solr**
* Search the Solr Index (Check all roles)

***

**Islandora Webform**

| Feature | anon. | authen. | admin. | webform mgr | webform submitter |
| ------------- |:-------------:| :-----:| :-------------: |:-------------:| :-----:|
| Manage Isl. Webforms      | - | - | X | - | - |
| Link Isl obj. to webforms | - | - | X | X | - |

***

**Islandora Webform Ingest**

| Feature | anon. | authen. | admin. | webform mgr | webform submitter |
| ------------- |:-------------:| :-----:| :-------------: |:-------------:| :-----:|
| Ingest Isl. Webform Submissions  | - | - | X | X | - |

***

**Webform (Drupal)**

| Feature | anon. | authen. | admin. | webform mgr | webform submitter |
| ------------- |:-------------:| :-----:| :-------------: |:-------------:| :-----:|
| Access all webform results | - | - | X | X | X |
| Access own webform results | - | - | X | X | X |
| Edit all webform submissions | - | - | X | X | - |
| Delete all webform submissions | - | - | X | X | - |
| Access own webform submissions | - | - | X | X | X |
| Edit own webform submission |  | - | X | X | X |
| Delete own webform submissions | - | - | X | X | X |
| Content authors: access and edit webofrm componenets and settings  |  |  | X | X | - |

***

## 8. Create and Configure Drupal accounts for users of the Islandora Webform module

* The IW module and IW Ingest modules automatically set some user permissions, but an administrator should verify that these permissions meet local needs.
* If a link to your webform is to be seen by only authenticated users, an administrator should set permissions to restrict webform access to authenticated users only.
* Create a Drupal account for those users according to local needs.
  * Username:  [username of individual user]
  * E-mail: [email address of the user]
  * Password: [any_dummy_password] (user can change this upon first use)
  * Status: Active (set to “Inactive” to prevent logging in)
  * Roles: webform submitter (has “authenticated” user permissions)

***

## 9. Enable the Drupal block for Islandora Webform submissions

* All of the submissions for an Islandora object can be made visible to users by configuring the IW block.
* Go to Administer > Structure > Blocks (ie. /admin/structure/block)
* In the section labeled "Disabled", find the block titled "Objects with Is Annotation Of relation" [The title may vary.]
* Select position: "Content" (Means place the block in the “content” region of the Drupal page.)
* Move the block to, say, the top of the "Content" list of items if you want the "Submission" link to be displayed at the top of the content block.
* Click "Save blocks"
* Click "configure" next to the "Objects with isAnnotationOf relation".

***

![webform_19.png](/docs/images/webform_19.png) [need image]

* Figure 4: Configuring the Drupal block of Islandora Webform submissions

***

  * Block title: "User Contributed Captions and Transcriptions". [example only]
  * View mode: "Links" [This is the most compact mode because it doesn't show thumbnails.]
  * Check "Only IW"
  * Pages (section)
    * Only the listed pages: [check]
    * In box: "islandora/object/*" (Means show this block only when displaying Islandora objects.)
  * Page Count: 10 (example only)
  * Roles (section)
    * authenticated user: [check] (make your own decision here whom to allow to see the submissions)
  * Click "Save".
* Click "Save blocks".

***

## 10. APPENDIX

### SOLR/GSearch Configuration

When a Fedora object is manually linked to a webform, enabling the link to appear on the object page, a new datastream "WF" is created on the object, containing a list of the node ids of all associated webforms. In order for a site builder to make a view of all objects that are associated with a given webform, SOLR and GSearch need to be configured to index the contents of that datastream.
**TODO -** Instructions for how to do this are needed!

***

### Enabling and configuring Islandora Webform Ingest

See the README.txt inside the submodules/islandora_webform_ingest folder for installation and usage instructions for the Islandora Webform Ingest module.

***

### Upgrading from an earlier version of the Islandora Webform module.

1. MAKE A DATABASE BACKUP. Upgrading this module may entail a number of database changes. If you encounter an error and need to downgrade, you must restore the previous database. You can make a database backup with your hosting provider, using the Backup and Migrate module, or from the command line.
2. Copy the entire islandora_webform directory the Drupal modules directory, replacing the old copy. DO NOT KEEP THE OLD COPY in the same directory or anywhere Drupal could possibily find it. Delete it from the server.
3. Login as an administrative user or change the $update_free_access in update.php to TRUE.
4. Run update.php (at http://www.example.com/update.php).

***

* An upgrade of these modules will not delete any webforms you have created (they are in mySQL), but your webforms may need to be tweaked to become compatible with the newer version of the IW module.
* When you upgrade the IW modules from an earlier version, do the following steps before cloning the new webform:
* Disable the Islandora webform module
```
> cd sites/all/modules (or wherever islandora_webform in located)
> drush cache-clear all
> drush dis islandora_webform (this also disables islandora_webform_ingest)
```
* If you need to wipe out IW completely and start over, run pm-uninstall after you disable the modules. This does not wipe out the webforms or the XML forms, but it will wipe out all your settings related to Islandora Ingest.
```
> drush pm-uninstall islandora_webform_ingest
> drush pm-uninstall islandora_webform
```
* Delete any existing “islandora_webform" directory and all its files.
* Before you delete the modules back up any files that you have customized in the Islandora Weborm directories. These will be overwritten when you upgrade the modules, such as:
```
islandora_webform/submodules/islandora_webform_ingest/examples/ .
      islandora_example_simple_text_solution_pack/xsl/modsrelated_to_dc.xsl
```
* When you are ready to delete all IW files, you can do this:
```
> rm -rf islandora_webform
> drush update
```

***
