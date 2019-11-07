# Use Case: Islandora Webform for Self-Deposit Workflows

The following is a general overview of what the IW module is capable of and how it is can be used to facilitate self-deposit of electronic theses and dissertations, faculty preprints, or other repository content.

## About the Islandora Webform Module

  * The ICG "Islandora Webform Module" (IW) is a Drupal module developed by Common Media/Born-Digital to add connectivity to an Islandora/Fedora repository.
  * The IW module is based on the Webform module that is compatible with Drupal 7.
  * The IW module works on any recent version of Islandora 7x.
  * The IW module can ingest any kind of object, from PDFs to images, audio, video, manuscripts, and books.
  * Because the IW module is built on the Drupal Webform module, it inherits all of the latter's functionality. The full functionality can be learned from online documentation and tutorials [see "Documentation" section below].
  * A working knowledge of MODS, XML Forms, XSLT, and the Fedora object architecture is necessary to set up and manage webforms and workflows.

## Configuring Self-Submit Forms

* Create a new webform (Content > Add Content > Webform)

### Islandora Settings
Configure how the form interacts with your Islandora collections at webform/islandora. **You must configure these settings before you will be able to map your form components to MODS metadata fields.**

* Select whether to allow the form to show up as a link on certain object pages. The typical self-deposit workflow will likely **not** use this option. 
* **Content Model Filter:** Select a content model for the objects this form will ingest. Each form can only support one content model, meaning a distinct form will be needed for each media type.
* **Collection Filter:** Select the collection where submissions will be deposited. Each form can only support deposit to one collection.
* **Islandora Embargo Settings:** Set global embargo settings for objects submitted via this form or select "Add embargo field to the form" to allow users to set their own embargo. 
* **Enable Ingest:** Select this option to allow users to upload files. Select the appropriate content model from the Ingest Destination drop-down. Set the relationship of your new object to the collection (typically isMemberOf), and define the appropriate namespace.

### Form Settings
The form settings menu at webform/configure allows administrators to set confirmation messages or redirects, set limits on the number of submissions, open and close the form, enable a progress bar and preview page, set the user roles that can access the webform, and a range of other form behaviors.

### Email Notifications
The emails menu at webform/emails allows administrators to specify who should receive email notifications when the form is submitted.

### Form Components
The form component menu is where administrators set up form fields and mappings to MODS. Webforms can be set up to map to any existing XML form.

* Type a label into the "New Component Name" field and select a field type. Click "add."
* Configure the label, form key, default value (optional), and description (optional).
* Configure validation and display preferences.
* Save the component.
* Next, go back and edit the component. A new section called "Islandora Ingest Mapping" has appeared!
* Select whether the metadata ingested from this field will replace any existing metadata in that field (if applicable) or be added as an additional field (append). Most field mappings will use "replace." The exception is if you are setting up multiple fields, for example author names. The first author should be set to "replace" while subsequent authors should be set to "append."
* Select the datastream that will be modified. For self-deposit use cases, this will typically be "MODS(cModel MODS form)" (e.g., MODS(Thesis MODS form).
* Select the MODS field that your webform field should map to (e.g., the Title field in your webform might map to titleInfo:title.
* Save your component and then save your form.

#### Form Component Tips

* If you are using the Islandora Scholar Profiles and Islandora Matomo modules, make sure to configure mappings to the u1 and u2 fields that enable linking person and organization entities to Islandora objects.
* The Islandora Object PID and Depositor Email fields will appear automatically after you configure the Islandora settings. These fields are required, but need not be displayed to end users. Tokens and default values are helpful here. For example, the Islandora Object PID default value should be the PID for the collection into which you are depositing. The Depositor Email field can be autopopulated for authenticated users.

### Setting up Webform Workflows

  * If the user is authorized to submit webforms and is logged in, they will 
  * The user then fills in the form and when done, clicks "Submit". 
  * The submission then goes into a temporary holding area waiting for a manager to approve it.
  * When a form is submitted, a manager can be notified by email. They can preview form submissions at admin/content/webform.
  * To ingest the submission, the manager must navigate to the Islandora collection to which the object was deposited. There will be a Submission tab (typically at /islandora/object/namespace:collectionname/submissions). Clicking "Ingest" allows the administrator to review and edit any metadata and files associated with the submission and then ingest it.
  
### Restricting Access

  * Forms can be set up to be visible only to specific user roles on the webform/configure menu.

## Documentation

  * [https://www.drupal.org/documentation/modules/webform/faq] (Drupal Webform FAQ
  * [https://www.drupal.org/node/2834425] (Drupal Webform Tutorials)
  * [https://www.drupal.org/node/2834432] (Drupal Webform Code Snippets)
  * [https://www.drupal.org/node/2834436] (Drupal Webform Theming)
  * [https://www.drupal.org/node/1526208] (Drupal Webform Related Projects)
  * [Overview: Islandora Webform](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/README.md)
  * [How to Install the Islandora Webform module](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_installation.md)
  * [Summary of Steps for Creating an Islandora Webform](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_steps.md)
  * [How to Create a Webform using the Islandora Webform module](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_creation.md)
  * [For End Users: How to Use an Islandora Webform](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_for_users.md)
