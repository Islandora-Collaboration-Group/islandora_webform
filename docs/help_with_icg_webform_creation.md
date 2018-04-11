# How to Create a Webform using the Islandora Webform module
(last updated: May 23, 2017, Peter MacDonald)

## Table of Contents

1. [Overview: Islandora Webform](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/README.md)
2. [How to Install the Islandora Webform module](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_installation.md)
3. [Summary of Steps for Creating an Islandora Webform](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_steps.md)
4. **[How to Create a Webform using the Islandora Webform module](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_creation.md)**  
    1. The "Islandora Webform Module"  
    1.1. Basic capabilities  
    1.2. Limitations  
    2. Creating a Drupal Webform  
    2.1. Add components to the webform  
    2.2. Configure the confirmation message  
    2.3. Configure AJAX and submission access
    3. Configuring the Islandora Settings for a webform  
    3.1. Specify which Islandora objects a webform targets  
    3.2. Specify how the webform values will be stored in Islandora  
    3.3. Specify the field mapping of webform values in the metadata  
    3.4. Configuring the "Simple Text Related Item MODS form" (XML form)
    4. APPENDIX: The "Islandora Simple Text Content Model"
5. [For End Users: How to Use an Islandora Webform](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_for_users.md)
6. [Use Case: Islandora Webform for Transcriptions (DHi@Hamilton College)](https://github.com/Islandora-Collaboration-Group/islandora_webform/blob/7.x/docs/help_with_icg_webform_transcriptions.md)

***

## 1. The "Islandora Webform Module"
### 1.1 Basic capabilities of the _Islandora Webform Module_

* The Islandora Webform (IW) module uses the capabilities of the standard Drupal Webform module to enable users to submit comments (captions, tags, transcriptions, etc.) on digital objects in an Islandora repository.
* In Islandora, a link to the webform appears on the page of qualifying repository objects. When clicked by an authorized visitor, the link launches the webform that gathers data from the user and puts the submitted data into a queue waiting for approval by a webform manager.
* When the submission is approved, the form values are ingested either 1) into the MODS (or another specific datastream) of the Fedora object being commented on, or 2) into a completely new Fedora object. If 2) is used, a relationship statement is placed in the RELS-EXT datastream connecting the submission Fedora object to the original Fedora object.
* All the submissions for a specific object can then be displayed along with the original object using a dedicated Drupal block.

***

### 1.2. Limitations of the _Islandora Webform Module_

* After a webform submission is ingested into Fedora, the IW “Submissions” page will shows “Re-Ingest” in a red font. It will also show links to “View”, “Edit” and “Delete” the submission. However, once the submission has been ingested, any edits made through this page will not replace the text already ingested into Fedora. So, if you do edit the text of the submission and you then click the red “Re-ingest” link, the IW module will create a brand new Fedora object -- leaving the original one still on place and unchanged. [**PETER: Verify what happens if you are ingesting into MODS -- does it replace or append or nothing?**]
* Implications of this are that it might be advisable to “Delete” a submission from the “Submissions” page once the ingest has been approved and ingested. “Delete” does not delete the Fedora object. It deletes onlythe entry for the submission on the Submissions page (a mySQL deletion).
* Another implication is that if you ever want to re-edit the content of a submission that was put into a MODS datastream by the IW module, you should do the editing using the XML metadata form originally used to create that object.
* You can associate a webform with only one content model at a time or no content model at all. So, if you are in the practice of ingesting objects of different MIME type (content models are bound to certaiin MIME types) into the same collection, you might be able to get away with not binding a webform to any content model at all. But if you want unique components in the webforms for each MIME type you will have to create separate Islandora webforms for each content model.


***

## 2. Creating a Drupal Webform

* Log in to the site (URL) where you want to create a webform. Be sure your account/role is permitted to create webforms.
* Administer > Content; Click Add content; Select "Webform" (i.e., /node/add/webform).
  * [If a webform has never been created before on this site, you might not see “webforms” in the list of content types. If "Webform" is not listed here, then your administrator will have to add it (see Administer > Structure > Content types (i.e., /admin/structure/types).
    * If you still do not see "Add Webform" go to /node/add/webform" directly on the URL line.
* Create Webform
  * Add values to the fields to the webform as follows:

***

![webform_01.png](/docs/images/webform_01.png)

* Figure 1: Creating a Drupal Webform

***

*  
  * Title: "IW PDF webform." (Customize this text to include the object content type (PDF, etc.) and/or the target collection name.
    * Actually, you can name it however you want -- depending on how narrowly or broadly you want the form to be used.
    * For example: "IW PDF webform for the Sacred Centers of India collection."
  * Comment settings (appears only of "comments" is enabled for your site):
    * Closed: [check] (The IW module works just fine with the built-in Comments set to “Closed”.]
  * Publishing options:
    * Published: [check]
    * Promoted to front page: [uncheck]
    * Sticky at top of lists: [uncheck]
* Click "Save" (button).

### 2.1. Add components to the webform

* If this is your first webform, you will now be at the Webform > Form components page.
  * But if you are returning to edit an existing webform, you will first need to select the webform that you want to work on: Administer > Content > Webforms (e.g., /admin/content/webform). Then find the title of your webform.

***

![webform_02.png](/docs/images/webform_02.png)
* Figure 2: Finding a Drupal webform

***

![webform_03.png](/docs/images/webform_03.png)

* Figure 3: Viewing the components of a Drupal webform

***

*  
  * Create as many components (fields) as you want to be displayed on the form for the user to fill out.

* For a new webform, the only component required is one for the submitted text (i.e., the “Caption”, “Transcription”, whatever).
  * Other components that you might find useful to configure are:
    * today’s date
    * user’s Drupal account ID
    * submission type (a select box offering, say “Caption”, “Transcription”, and “Other”
  * but the IW module only requires a field to hold the submitted text. The rest are optional and should be determined by your needs.
  * Enter values for each desired component of the form (the following are example values only)
    * Label: "Caption or Transcription" (or, maybe, just “Submission text”)
    * Type: select "Textarea" (“textarea” provides a larger area for text entry than “textfield” does.)
    * Value: [blank] (You will edit this field later.)
    * Required: [check]
  * Click "Add" (button) to save each component as you create it.
* When you click “Add”, you will be taken to a page where you should continue configuring the component you just chose to Add.

***

![webform_04.png](/docs/images/webform_04.png)

* Figure 4: Editing a component of a Drupal webform

***

*  
  * Configure this component
    * Label: [prepopulated] (Will show the Label you previously specified for this component.)
    * Field Key: [prepopulated] (Will be created automatically from the component's Label.)
    * Default value (optional):  [Leave blank and use the “Placeholder” setting discussed below.]

![webform_04b.png](/docs/images/webform_04b.png)

* Figure 5: Configuring a webform component (part 1 or 2)

  *  
    * Description: [Usually you will just leave this blank, but you can add text if more information is needed.]
      * Note: Most HTML tags are ignored in the Description box.
      * Example (using “Filtered HTML” since it looks like "Full HTML" is not allowed):
```
<strong>Instructions</strong>: HTML tags you may use: &lt;a&gt; &lt;strong&gt; &lt;p&gt; and &lt;br&gt;. [If you are doing a transcription, please follow the <a href="/transcription-instructions" target="_blank">Transcription instructions</a>]
```

***

![webform_05.png](/docs/images/webform_05.png)

* Figure 6: Configuring a webform component (part 2 of 2)

***

** Validation**
* Required: [check]

* *Display**
* Resizable: [check] [This option may not be visible on all versions of the webform.]
* Label display: select "Above" [can be changed later]

** Placeholder**
* Supply message (optional): “-- Put your input here. --”

[Other options may be left blank for now.]

* Click "Save component" (button).
* Now you can add more components until you have added all the ones you want, such as "First Name", "Last Name", "Drupal user account", "Date" and so on. If you create custom fields, you will later have to modify the metadata XML form to ensure there is a matching MODS element there you can map the component to.
* When done adding components, click "Save" (button) to save the whole webform. This is very important, but easy to forget.

### 2.2. Configure the confirmation message

* Be sure you are on the “Form settings” page.
  * Help finding "Form settings" page
    * Administer > Content > Webforms (i.e., /admin/content/webform)
    * Find your webform, click the "Edit" link, then click the "Webform" tab, then click the "Form settings" button.

***

![webform_06.png](/docs/images/webform_06.png)

* Figure 7: Configuring the confirmation message of a webform

***

* Submission Settings
  * Confirmation message: "Thank you for submitting your caption." (example only)
    * You can add Drupal tokens in the confirmation message, e.g.
      * “Thank you [current-user:name] for your submission.”
      * “[current-user:name]” is a Drupal token you can use here if you want.
  * Text format: "Full HTML" (your text format options may differ -- depending on how "text formats" are set up on your Drupal instance)
    * Help configuring Text formats:
      * Administer > Configuration > Content authoring > Text formats  (i.e., /admin/config/content/formats).
  * “Full HTML” allows you to use any HTML tags you want.
  * Redirection location
    * Confirmation page: [uncheck]
    * Custom URL: [uncheck and leave box blank]
    * No redirect (reload current page): [check] [The other two options cause navigation problems for users.]
* Fill in any remaining options as needed, but the defaults should all work fine.

### 2.3. Configure AJAX and submission access

* Inline Islandora Webform [If this fieldset is missing, it is because the webform_ajax" module has not been installed.]

***

![webform_07.jpg](/docs/images/webform_07.jpg)

* Figure 8: Configuring AJAX for the Drupal webform

***

*  
  * Inline AJAX mode: [check] (AJAX is used to put the webform inline with the original (target) object being commented on.
  * Show confirmation screen: [check, if visible] (Possible bug: this has been known to generate duplicate confirmation messages, but unchecking it may generate an error.)

* **Submission Access - Roles that can submit this webform**
  * anonymous user: [uncheck]
  * authenticated user: [uncheck]
    * [Unless, of course you actually do decide to let all authenticated users make a submission.]
  * administrator: [check]
  * webform manager: [check]
  * webform submitter: [check]
* Fill in any remaining options as needed, but the defaults should all work fine.

* **Progress Bar [no changes required]**
* **Preview Page**
  * Enable preview page: [leave this unchecked] (Do not enable this without thoroughly testing the user experience.)
* **Advanced Settings**
  * Accept the defaults settings for now because no best practices exist for these settings yet.
* Click "Save configuration" (button).

***

## 3. Configuring the Islandora Settings for a webform

* WARNING: Before you change anything on this page, be aware that if the webform you are configuring has already been used for any submissions and those submissions have not yet been ingested, those submissions might not ingest properly. As a precaution, you should ingest all pending webforms submissions before making any changes to an existing webform’s Islandora settings. If you are creating a new webform, you don't have to worry about this.

### 3.1. Specify which Islandora objects a webform targets

  * Ensure you are on the “Form settings” page, if not…
  * Go to Administer > Content > Webforms (i.e., /admin/content/webform)
  * Find the title of your webform and click the "Components" link.
* Click the Webform (tab) and then "Islandora settings" (button)

***

![webform_08.png](/docs/images/webform_08.png)

* Figure 9: Configuring the _Islandora Settings_ for a webform (part 1 of 2)

***

* **Islandora Options**
  * Enabled: [check]
  * Content model filter: [Select one of the options]
* This setting determines which Islandora pages will have a link to this webform on them.
* The drop-down list shows the names of all content models it finds in Fedora. The labels shown come from the Label property of the Fedora object itself.
  * “Any” [Do not select this option. This would place a webform link on every object --  including collection objects and under thumbnails of every object in a search result set.]
  * Example: "Islandora PDF Content Model"
* Collection filter: [Select one of the options.]
  * "Any" (This option means this form can be used in any collection/site.)
  * [It is recommended that you not set this to “Any” on a production site.]
* The drop-down list shows the titles of all "collection" objects (MIME-type collections and content collections and "sub-collection" objects). The labels shown come from the Label property of the Fedora object itself.
  * "American Prison Writing Project"
* Specifying a collection means that this form will be offered as a link on objects only in that collection.
* Be sure to select a collection that has objects that subscribe to the content model you specified for the “Content model filter.”
  * PID Search String: [leave blank if all objects that qualify according to the above criteria.]
    * regex options are: [no example available]
      * [COMMENT: We need examples of this to guide us, try "sci:1*, lib*]
  * Add a link to...: [select one of the two option below]
    * "All matched objects" (Means a link to the webform will appear on all objects as defined on this page.)
    * "Only those objects that are manually tagged for this webform"
* If you select this, you will later need to select each object that should have a link to the webform. (see section “”##).
  * Link Text: "Add a caption for this item." [example only]
    * Other examples: "Transcribe this item." or a general use one like “Comment on this item.”
  * Link help text: "Your input is greatly appreciated." [example only]

** Enabling webforms on only certain objects**

* If you want links to the webform to appear on only Islandora pages for only certain objects, you need to indicate this when editing the “Islandora settings” page. Look for “Add a link to” and select "Only those objects that are manually tagged for this webform".
* Then you need to navigate to each Islandora object display page on which you want a link to the webform to appear and click the link to force the appearance of a link to a webform on that specific object page.
  * add webform link: "Go to web form" (wording may vary)

***

### 3.2. Specify how the webform values will be stored in Islandora

* Purpose: to specify where the component’s content will be stored in the Fedora object.
* The IW module doesn't predetermine which components (i.e. form fields) you use in your webform. This is determined by your needs, but whatever components you choose must be mapped to MODS element paths in the specified metadata XML form. however, the crosswalk that comes with the IW module crosswalks <mods:title> to <dc:relation> -- not <dc.title>. DHi@Hamilton changed this crosswalking XSLT so it maps it to <dc:title>. (But this is not the place for a full explanation of how to do this.)

* **Islandora Ingest**
  * Enable: [check]
  * Ingest destination: Select "Create new Islandora Simple Text Content Model" (Means create a Fedora object that subscribes to the islandora:sp_example_text content model. For more about this important content model see the APPENDIX below.)

***

![webform_09.png](/docs/images/webform_09.png)

* Figure 10: Configuring the _Islandora Settings_ for the Drupal webform (part 2 of 2)

***
* If you do not see "Create new Islandora Simple Text Content Model", save this page (first uncheck "Enable" though) and ask the administrator to download/install/enable the "Islandora Example Simple Text Solution Pack". The Fedora content model object for the Solution Pack has to be installed too. Then come back and finish these instructions.
  * Relationship to current object: Select "is Annotation Of"
    * [WARNING: Do not use the predicate “isMemberOfCollection”. Doing so brings the risk of relationships corruption throughout your Fedora database.]
* At the moment, the "master" branch in IW is configured to use the default Fedora ontology only.
* There is a proposed enhancement to allow configuring any standards-based ontology here, but it is not yet an official project.
* You should consult the default fedora ontology for the meaning of the various predicates in this list:
* Fedora Digital Object Relationships (Fedora 3.x)
  * http://www.fedora-commons.org/documentation/3.0/userdocs/digitalobjects/introRelsExt.html
* Fedora Object Ontology (Fedora 3.x)
  * http://www.fedora.info/definitions/1/0/fedora-relsext-ontology.rdfs
* Namespace of new object: "apwiw" [example only]
* Ensure that whatever namespace you choose is allowable in the collection you specified above under "Collections filter". [NOTE: Not sure this is true. That is, does Solr need it for searching. Still TBD.]
* Click "Save configuration" (button).

***

### 3.3. Specify the field mapping of webform values in the metadata

* Navigate to the Webform "Form components" page:
* Administer > Content > Webforms (tab) > find the title of your webform and click "Components" (link).
* Find the component that will store the text of the submission and click the "Edit" link.

***

![webform_10.png](/docs/images/webform_10.png)

* Figure 11: Configuring the Islandora Ingest field mapping for a webform

***

*  
  * TROUBLESHOOTING: If you get this message: "Islandora Simple Text Content Model provides no datastreams populated from this webform."
  1) You might not have yet set the association of the "Simple Text Related Item MODS form. or
  2) You might have forgotten to enable the "Islandora example simple text module."
  * Ingest?: Select "Append" [This is the preferred option. The other options are: "Do not ingest" and "Replace"]
  * DataStream: "Select MODS" (The datastream labels listed here are those of the “Islandora Simple Text Content Model” which is used by Islandora for creating the caption object in Fedora.)
  * Field:
    * For a "Title" component, select "relatedItems:relTitleInfo:relTitle (text/plain)" [Select the path to the MODS form element where you want the main webform text to be stored. The default XML form is "Simple Text Related Item MODS form".]
    * For a "Transcription" component, select, say, "relatedITems:noteTab:0:noteText (text/plain)."
* Click "Save component" (button)
* Now you can proceed to map all the remaining components (except for the "Islandora object PID") to MODS fields in the XML form. Here are some example mappings from one of DHi's forms:
  * Submission - relatedItems:noteTab:0:noteText (text/plain)
  * Type of Submission - relatedItems:relatedGenre (text/plain)
  * Short title - relatedItems:relTitleInfo:relTitle (text/plain)
  * Last Name - relatedItems:relNameInfo:family (text/plain)
  * First Name - relatedItems:relNameInfo:given (text/plain)
  * User Account - relatedItems:relNameInfo:affiliation (text/plain)
  * Date - relatedItems:relOriginInfo:relDateCreated (text/plain)

**The Islandora object PID component**

* At this point, the Islandora Webform module has silently added a new component to your webform labeled "Islandora object PID".
* The "Islandora object PID" component holds the PID of the parent Islandora object. It does not need to be edited manually. It is automatically generated and inherits some settings from other components in the same webform. Upon ingest of a submission, the IW code stores the parent PID in the RELS-EXT datastream of the new object. Example:
```
<fedora:isAnnotationOf rdf:resource="info:fedora/islandora:1">
</fedora:isAnnotationOf>
```
* (Optional) Examine this new component
* Administer > Content > Webforms (i.e. /admin/content/webform)
* Find the title of your webform > click "Components", and you will see the component "Islandora object PID".
  * Label: "Islandora object PID"
  * Type: "Textfield"
  * Value: -
  * To see more of its fields, click "Edit".
    * Label: "Islandora object PID" [do not change]
    * Field Key: "islandora_object_pid" [do not change]

***

![webform_11.png](/docs/images/webform_11.png)

* Figure 12: The Islandora object PID component of a webform

***

### 3.4. Configuring the "Simple Text Related Item MODS form" (XML form)

* The IW modules comes with a preferred XML form (Simple Text Related Item MODS form), which is configured to work with the module’s PHP code and the supplied content Model (islandora:sp_example_text). This XML form is designed to hold information on a “related item.” The Fedora object holding the submission information is treated as the related item and all the submission information is wrapped in the <mods:relatedItem> element. [You can configure this differently on your site.]
* If you decide to just use the default XML form, you still need to set its association with the Content Model set in the Islandora Settings in your webform
* It is very possible that the supplied XML form will not meet all of your metadata needs and you will need to customize it. For example,
  * At DHi@Hamilton we unchecked “Access” to a few MODS elements that we did not need, such as, subTitle, reldisplayLabel, roleText, roleCode, and noteDisplaylabel.
  * DHi replaced the built-it controlled vocabulary for the “type” attribute with its own terms:: Caption, Tags,Transcription, Translation, Question, Suggestion.
  * DHi changed the mapping of the attribute “type” from reldisplayLabel to a full MODS element <mods:genre> so we could more easily retrieve it from Solr. This required adding crosswalk from mods:genre to dc:type in the transform: modsrelated_to_dc.xsl.
  * DHi added a dateCreated element in the XML form.
* Make any local changes to the Simple Text Related Item MODS form
* Since editing built-in XML forms is not allowed in Islandora, the "Simple Text Related Item MODS form" has to be cloned (copied) if you decide you need to customize it.
    * Use case: If you want to change the field type of the main input component from, say, "textfield", to "textarea".
    * “Copy” the built-in XML form to a new one.
    * Open your new XML form.
    * Select the title element: relatedItems:relTitleInfo:relTitle
    * Change "Type" to "textarea".
    * Click "Save".
    * Associate the new XML form with the Simple Text Content Model (admin/islandora/xmlform)
    * Find "<your_new-XML-form>” and click "Associate".
    * Click “Add Association”
      * Content Model:  select "Islandora Simple Text Content Model (islandora:sp_example_text)"
      * Metadata Datastream ID: "MODS"
      * Title Field: "['relatedItems']['relTitleInfo']['relTitle']"
      * XSL Transform: "modsrelated_to_dc.xsl" [or modsrelated_to_dc.xsl?]
      * Self XSL Transform: "No transform"
      * Upload Template Document: [optional]
      * Click "Add Association" (button)
    * Be sure to "Disable" the built-in XML form if you are not going to use it in production.
    * Make the same change in the configuration of the matching component in your webform.
* Locate the webform that should be set up to work with the XML form you just modified.
* Click the "Components" button.
  * Find the main text submission component and verify that it's TYPE is set to "textarea". If it is not create a new component that is and delete the one that is not.
  * Click "Edit" next to the main text submission input component and then finish configuring that component’s “Islandora Ingest Mapping”.
  * Open "Islandora Ingest Mapping" fieldset.
  * If you get the message “Islandora Simple Text Content Model provides no datastreams that can be populated from this webform field type.” then you selected the wrong Content Model when you set up the XML Form.
  * Ingest: "Append"
  * DataStream: "MODS" (Note that the drop-down entry shows also the name of the webform itself, as in “MODS (My IW webform)”.
  * Field: ['relatedItems']['relTitleInfo']['relTitle']
    * Or select another path that corresponds to the component you are adding.
    * For "surname" map to DataStream:  "relatedItems:relNameInfo:namePart:family
  * Click "Save component".

***

4. APPENDIX: The _Islandora Simple Text Content Model_

* The IW module comes bundled with the "Islandora Example Simple Text Solution Pack", which is installed when you install the main Islandora Webform module. The IW Simple Text SP comes with its own content model, the “Islandora Simple Text Content Model”. This content model processes the values submitted by a webform. Studying the content model will help you configure the webform components properly. So spend some time examining the content model and the XML metadata form associated with it, the "Simple Text Related Item MODS form."
* The code (in Islandora Webform Ingest) supporting this CM grabs a form’s output and creates a Fedora object (or writes to a datastream in the original object) and plugs in those values in a programmatic way into a MODS datastream which gets crosswalked to Dublin Core (DC). Even the MODS to DC crosswalk can be configured in an XSLT file to meet your local needs.
* By default, the MODS datastream in Fedora created by the IW Module from a webforms values might look something like this...
```
<?xml version="1.0"?>
<mods xmlns="http://www.loc.gov/mods/v3"
xmlns:mods="http://www.loc.gov/mods/v3"
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
xmlns:xlink="http://www.w3.org/1999/xlink"
xsi:schemaLocation="http://www.loc.gov/mods/v3
http://www.loc.gov/standards/mods/v3/mods-3-4.xsd">
 <relatedItem type="host" xlink="http://lis.hamilton.edu/islandora/object/islandora%3A1">
    <titleInfo lang="" displayLabel="">
     <title>This photograph was probably taken in Toledo, Ohio.</title>
     <subTitle/>
    </titleInfo>
    <name type="personal">
     <namePart>Peter MacDonald</namePart>
    </name>
 </relatedItem>
</mods>
```

* This MODS is crosswalted to a DC datastream (inline, text/xml) that might look something like this:
```
<oai_dc xmlnsdc::oai_dc="http://www.openarchives.org/OAI/2.0/oai_dc/" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.openarchives.org/OAI/2.0/oai_dc/ http://www.openarchives.org/OAI/2.0/oai_dc.xsd">
<dc:title>This photograph was probably taken in Toledo, Ohio</dc:title>
 <dc:contributor>Peter, MacDonald</dc:contributor>
 <dc:identifier>apw:235</dc:identifier>
<dc:date>2016-01-26</dc:date>
 <dc:description>This photograph was probably taken in Toledo, Ohio. </dc:description>
</oai_dc:dc>
```
* The code puts an RDF triple pointing to the PARENT_PID in the RELS-EXT datastream adds.
* RELS-EXT (inline, applications/rdf+xml)
```
<fedora:isAnnotationOf rdf:resource="info:fedora/yourNamespace:1">
</fedora:isAnnotationOf>
```

* Islandora Simple Text Content Model (islandora:sp_example_text)
```
<dsCompositeModel xmlns="info:fedora/fedora-system:def/dsCompositeModel#">
 <dsTypeModel ID="DC">
     <form FORMAT_URI="http://www.openarchives.org/OAI/2.0/oai_dc/" MIME="application/xml"></form>
 </dsTypeModel>
 <dsTypeModel ID="RELS-EXT" optional="true">
     <form FORMAT_URI="info:fedora/fedora-system:FedoraRELSExt-1.0" MIME="application/rdf+xml"></form>
 </dsTypeModel>
 <dsTypeModel ID="RELS-INT" optional="true">
     <form FORMAT_URI="info:fedora/fedora-system:FedoraRELSInt-1.0" MIME="application/rdf+xml"></form>
 </dsTypeModel>
 <dsTypeModel ID="MODS" optional="true">
    <form FORMAT_URI="http://www.loc.gov/mods/v3" MIME="application/xml"></form>
 </dsTypeModel>
 <dsTypeModel ID="HTML" optional="true">
     <form MIME="text/html"></form>
    </dsTypeModel>
 <dsTypeModel ID="TN" ORDERED="false" optional="true">
    <form MIME="image/jpg"></form>
    <form MIME="image/jpeg"></form>
    <form MIME="image/png"></form>
 </dsTypeModel>
</dsCompositeModel>
```

***
