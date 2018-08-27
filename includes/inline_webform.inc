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
 * @param string $ajax
 *   Indicates if the link is to be "ajaxified"
 *
 * @param mixed $submission
 *   A submission object to pass to the webform_client_form callback. If present,
 *   the form will display the values from that submission, and submitting will
 *   update that submission rather than create a new one.
 *
 * @return array
 *   ajax commands
 */
function _iw_inline_webform($webform, $ajax, $submission = FALSE) {
  $enabled = TRUE;
  $preview = FALSE;
  if(!empty($submission->islandora_object)) {
    $webform->webform['islandora_object'] = $submission->islandora_object;
  }
  $submission_is_draft = !empty($submission->is_draft);
  $form = drupal_get_form('webform_client_form_' . $webform->nid, $webform, $submission, $submission_is_draft);
  if ($ajax == 'ajax') {
    // We remove the container of the form.
    $commands['remove-container'] = ajax_command_remove('.webform-links-wrapper.opened > div:not(.webform-links)');
    // Set css on webform links wrapper.
    $commands['remove-wrapper-class'] = ajax_command_invoke('.webform-links-wrapper', 'removeClass', array('opened'));
    $commands['add-wrapper-class'] = ajax_command_invoke('#iw_webform_links_' . $webform->nid . '.webform-links-wrapper', 'addClass', array('opened'));
    // Get the html of the new form.
    $html = render($form);
    $jquery_selector = '#iw_webform_links_' . $webform->nid . ' .webform-links';
    $commands['jquery-selector'] = ajax_command_after($jquery_selector, $html);
    $vars = array('webform' => $webform, 'form' => $form, 'html' => $html);
    drupal_alter('iw_inline_webform_ajax_commands', $commands, $vars);
    return array(
      '#type' => 'ajax',
      '#commands' => $commands,
    );
  }
  else {
    return $form;
  }
}
