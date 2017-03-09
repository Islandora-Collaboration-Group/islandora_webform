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
    // We remove the container of the form.
    $commands['remove-container'] = ajax_command_remove('.webform-links-wrapper.opened > div:not(.webform-links)');
    // Set css on webform links wrapper.
    $commands['remove-wrapper-class'] = ajax_command_invoke('.webform-links-wrapper', 'removeClass', array('opened'));
    $commands['add-wrapper-class'] = ajax_command_invoke('#iw_iwebform_links_' . $webform->nid . '.webform-links-wrapper', 'addClass', array('opened'));
    // Get the html of the new form.
    $html = render($form);
    $jquery_selector = '#iw_iwebform_links_' . $webform->nid . ' .webform-links';
    $commands['jquery-selector'] = ajax_command_after($jquery_selector, $html);
    $vars = array('webform' => $webform, 'form' => $form, 'html' => $html);
    drupal_alter('iw_inline_webform_ajax_commands', $commands, $vars);
    return array(
      '#type' => 'ajax',
      '#commands' => $commands,
    );
  }
  else {
    return render($form);
  }
}
