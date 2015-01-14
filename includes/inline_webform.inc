<?php

/**
 * @file
 * Provides inline islandora webform functions.
 *
 */

/**
 * Ajax callback to render inline webform.
 *
 * @param object $webform
 *   webform object
 *
 * @return array
 *   ajax commands
 */
function _iw_inline_webform($webform) {
  // Remove any open webform forms.
  $commands[] = ajax_command_remove("form.webform-client-form");

  // Set css on webform links wrapper.
  $commands[] = ajax_command_invoke('.webform-links-wrapper', 'removeClass', array('opened'));
  $commands[] = ajax_command_invoke('#iw_iwebform_links_' . $webform->nid . '.webform-links-wrapper', 'addClass', array('opened'));

  // Get the html of the new form.
  $submission = FALSE;
  $enabled = TRUE;
  $preview = FALSE;
  $html = render(drupal_get_form('webform_client_form_' . $webform->nid, $webform, $submission, $enabled, $preview));

  // Insert the webform.
  $jquery_selector = '#iw_iwebform_links_' . $webform->nid . ' .webform-links';
  $commands[] = ajax_command_after($jquery_selector, $html);

  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );

}