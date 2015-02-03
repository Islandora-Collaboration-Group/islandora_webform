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
function _iw_inline_webform($webform, $ajax) {
  $submission = FALSE;
  $enabled = TRUE;
  $preview = FALSE;
  $form = drupal_get_form('webform_client_form_' . $webform->nid, $webform, $submission, $enabled);
  if ($ajax == 'ajax') {
    $commands[] = ajax_command_remove('.webform-links-wrapper.opened > div:not(.webform-links)'); //we remove the container of the form.
  // Set css on webform links wrapper.
    $commands[] = ajax_command_invoke('.webform-links-wrapper', 'removeClass', array('opened'));
    $commands[] = ajax_command_invoke('#iw_iwebform_links_' . $webform->nid . '.webform-links-wrapper', 'addClass', array('opened'));
  // Get the html of the new form.
    $html = render($form);
    $jquery_selector = '#iw_iwebform_links_' . $webform->nid . ' .webform-links';
    $commands[] = ajax_command_after($jquery_selector, $html);
    return array(
      '#type' => 'ajax',
      '#commands' => $commands,
    );
  }
  else {
    return render($form);
  }
}
