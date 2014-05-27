<?php

/**
 * @file
 *
 * This is the task handler plugin to handle generating 403, 404 and 301 response codes.
 */

// Plugin definition
$plugin = array(
  // is a 'context' handler type, meaning it supports the API of the
  // context handlers provided by ctools context plugins.
  'handler type' => 'context',
  'visible' => TRUE, // may be added up front.

  // Administrative fields.
  'title' => t('Symfony route collection'),
  'admin summary' => 'symfony_route_collection_admin_summary',
  'admin title' => 'symfony_route_collection_title',
  'operations' => array(
    'settings' => array(
      'title' => t('General'),
      'description' => t('Change general settings about this variant.'),
      'form' => 'symfony_route_collection_edit_settings',
    ),
    'criteria' => array(
      'title' => t('Selection rules'),
      'description' => t('Control the criteria used to decide whether or not this variant is used.'),
      'ajax' => FALSE,
      'form' => array(
        'order' => array(
          'form' => t('Selection rules'),
        ),
        'forms' => array(
          'form' => array(
            'include' => drupal_get_path('module', 'ctools') . '/includes/context-task-handler.inc',
            'form id' => 'ctools_context_handler_edit_criteria',
          ),
        ),
      ),
    ),
    'context' => array(
      'title' => t('Contexts'),
      'ajax' => FALSE,
      'description' => t('Add additional context objects to this variant that can be used by the content.'),
      'form' => array(
        'order' => array(
          'form' => t('Context'),
        ),
        'forms' => array(
          'form' => array(
            'include' => drupal_get_path('module', 'ctools') . '/includes/context-task-handler.inc',
            'form id' => 'ctools_context_handler_edit_context',
          ),
        ),
      ),
    ),
  ),

  // Callback to render the data.
  'render' => 'symfony_route_collection_render',

  'add features' => array(
    'criteria' => t('Selection rules'),
    'context' => t('Contexts'),
  ),
  // Where to go when finished.
  'add finish' => 'settings',

  'required forms' => array(
    'settings' => t('Panel settings'),
  ),

  'edit forms' => array(
    'criteria' => t('Selection rules'),
    'settings' => t('General'),
    'context' => t('Contexts'),
  ),
  'forms' => array(
    'settings' => array(
      'form id' => 'symfony_route_collection_edit_settings',
    ),
    'context' => array(
      'include' => drupal_get_path('module', 'ctools') . '/includes/context-task-handler.inc',
      'form id' => 'ctools_context_handler_edit_context',
    ),
    'criteria' => array(
      'include' => drupal_get_path('module', 'ctools') . '/includes/context-task-handler.inc',
      'form id' => 'ctools_context_handler_edit_criteria',
    ),
  ),
  'default conf' => array(
    'title' => t('Symfony route'),
    'contexts' => array(),
    'relationships' => array(),
    'route' => '',
    'environment' => 'prod',
    'debug' => FALSE,
  ),
);

function symfony_route_collection_admin_summary($handler, $task, $subtask, $page, $show_title = TRUE) {
  $task_name = page_manager_make_task_name($task['name'], $subtask['name']);
  $output = '';

  ctools_include('context');
  ctools_include('context-task-handler');

  // Get the operations
  $operations = page_manager_get_operations($page);

  // Get operations for just this handler.
  $operations = $operations['handlers']['children'][$handler->name]['children']['actions']['children'];
  $args = array('handlers', $handler->name, 'actions');
  $rendered_operations = page_manager_render_operations($page, $operations, array(), array('class' => array('actions')), 'actions', $args);

  $plugin = page_manager_get_task_handler($handler->handler);

  $object = ctools_context_handler_get_task_object($task, $subtask, $handler);
  $context = ctools_context_load_contexts($object, TRUE);

  $access = ctools_access_group_summary(!empty($handler->conf['access']) ? $handler->conf['access'] : array(), $context);
  if ($access) {
    $access = t('This panel will be selected if @conditions.', array('@conditions' => $access));
  }
  else {
    $access = t('This panel will always be selected.');
  }

  $rows = array();

  $type = $handler->type == t('Default') ? t('In code') : $handler->type;
  $rows[] = array(
    array('class' => array('page-summary-label'), 'data' => t('Storage')),
    array('class' => array('page-summary-data'), 'data' => $type),
    array('class' => array('page-summary-operation'), 'data' => ''),
  );

  if (!empty($handler->disabled)) {
    $link = l(t('Enable'), page_manager_edit_url($task_name, array('handlers', $handler->name, 'actions', 'enable')));
    $text = t('Disabled');
  }
  else {
    $link = l(t('Disable'), page_manager_edit_url($task_name, array('handlers', $handler->name, 'actions', 'disable')));
    $text = t('Enabled');
  }

  $rows[] = array(
    array('class' => array('page-summary-label'), 'data' => t('Status')),
    array('class' => array('page-summary-data'), 'data' => $text),
    array('class' => array('page-summary-operation'), 'data' => $link),
  );

  $link = l(t('Edit'), page_manager_edit_url($task_name, array('handlers', $handler->name, 'criteria')));
  $rows[] = array(
    array('class' => array('page-summary-label'), 'data' => t('Selection rule')),
    array('class' => array('page-summary-data'), 'data' => $access),
    array('class' => array('page-summary-operation'), 'data' => $link),
  );

  $link = l(t('Edit'), page_manager_edit_url($task_name, array('handlers', $handler->name, 'settings')));
  $rows[] = array(
    array('class' => array('page-summary-label'), 'data' => t('Environment')),
    array('class' => array('page-summary-data'), 'data' => $handler->conf['environment']),
    array('class' => array('page-summary-operation'), 'data' => $link),
  );

  $info = theme('table', array('header' => array(), 'rows' => $rows, 'attributes' => array('class' => array('page-manager-handler-summary'))));

  $title = $handler->conf['title'];
  if ($title != t('Panel')) {
    $title = t('Panel: @title', array('@title' => $title));
  }

  $output .= '<div class="clearfix">';
  if ($show_title) {
  $output .= '<div class="handler-title clearfix">';
    $output .= '<div class="actions handler-actions">' . $rendered_operations['actions'] . '</div>';
    $output .= '<span class="title-label">' . $title . '</span>';
  }

  $output .= '</div>';
  $output .= $info;
  $output .= '</div>';

  return $output;
}

/**
 * Set up a title for the panel based upon the selection rules.
 */
function symfony_route_collection_title($handler, $task, $subtask) {
  if (isset($handler->conf['title'])) {
    return check_plain($handler->conf['title']);
  }
  else {
    return t('Symfony route collection');
  }
}

/**
 * General settings for the panel
 */
function symfony_route_collection_edit_settings($form, &$form_state) {
  $conf = $form_state['handler']->conf;
  $form['title'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['title'],
    '#title' => t('Administrative title'),
    '#description' => t('Administrative title of this variant.'),
  );

  $form['environment'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['environment'],
    '#title' => t('Environment'),
    '#description' => t('The environment'),
  );

  $form['debug'] = array(
    '#type' => 'checkbox',
    '#default_value' => $conf['debug'],
    '#title' => t('Debug'),
    '#description' => t('Whether to enable debugging or not'),
  );

  return $form;
}

function symfony_route_collection_edit_settings_submit($form, &$form_state) {
  $form_state['handler']->conf['title'] = $form_state['values']['title'];
  $form_state['handler']->conf['environment'] = $form_state['values']['environment'];
  $form_state['handler']->conf['debug'] = !empty($form_state['values']['debug']);
}

function symfony_route_collection_render($handler, $base_contexts, $args, $test = TRUE) {
  // Go through arguments and see if they match.
  ctools_include('context');
  ctools_include('context-task-handler');

  // Add my contexts
  $contexts = ctools_context_handler_get_handler_contexts($base_contexts, $handler);

  // Test.
  if ($test && !ctools_context_handler_select($handler, $contexts)) {
    return;
  }

  if (isset($handler->handler)) {
    ctools_context_handler_pre_render($handler, $contexts, $args);
  }

  $kernel = symfony_route_collection_kernel($handler->conf['environment'], $handler->conf['debug']);

  $factory = function ($query, $request, $attributes, $cookies, $files, $server, $content) use ($base_contexts) {
    unset($query['q']);
    /** @var \Symfony\Component\Routing\RouteCollection $routes */
//    $attributes['_q'] = $base_contexts[''];
    dpm($base_contexts);
    return new \DrupalRequest($query, $request, $attributes, $cookies, $files, $server, $content);
  };

  \DrupalRequest::setFactory($factory);
  $request = \DrupalRequest::createFromGlobals();
  $response = $kernel->handle($request);
  $kernel->terminate($request, $response);

  $info = array(
    'response code' => $response->getStatusCode(),
    'fragment' => '',
    'destination' => '',
    'query' => array(),
    'content' => $response->getContent(),
    'no_blocks' => true,
  );

  return $info;
}

function symfony_route_collection_kernel($environment, $debug) {
  $loader = require_once __DIR__ . '/../../app/bootstrap.php.cache';
  \Symfony\Component\Debug\Debug::enable();

  require_once __DIR__ . '/../../app/AppKernel.php';

  $kernel = new AppKernel($environment, $debug);
  $kernel->loadClassCache();
  return $kernel;
}
