<?php
/**
 * @file Drush Module Version Management commands
 */

/**
 *
 */
function vm_drush_help($section) {
    switch ($section) {
        case 'meta:vm:title':
            return dt('Module Version commands');
        case 'meta:vm:summary':
            return dt('Run an update callback of a module or all installed modules.');
    }
}


/**
 *
 */
function vm_drush_command()
{
    $items['versionmanager-information'] = array(
        'callback' => 'drush_vm_information',
        'description' => 'Print information about the specified module(s).',
        'aliases' => array('vm-info'),
        'examples' => array(
            'drush versionmanager-information ' =>
            'Display information about any users with uids, names, or mail addresses matching the strings between commas.',
        ),
        'arguments' => array(
            'modules' => 'A comma delimited list of module names',
        ),
        'required-arguments' => true,
        'options' => array(
            'full' => 'show extended information about the module',
            'short' => 'show basic information about the module (this is the default)',
        ),
    );

    $items['versionmanager-update'] = array(
        'callback' => 'drush_vm_run',
        'description' => 'Print information about the specified module(s).',
        'aliases' => array('vm-update'),
        'examples' => array(
            'drush versionmanager-information ' =>
            'Display information about any users with uids, names, or mail addresses matching the strings between commas.',
        ),
        'arguments' => array(
            'modules' => 'A comma delimited list of module names',
        ),
        'required-arguments' => true,
        'options' => array(
            'all' => 'Run the update on all installed and enabled modules',
        ),
    );

    return $items;
}

/**
 * Prints information about the specified module(s).
 *
 * @param array $modules
 */
function drush_vm_information($modules)
{
    // get installed modules
    // display retrieved information
}

/**
 * Runs the update of each specified module.
 *
 * @param array $modules
 */
function drush_vm_run($modules)
{
    // drush_get_option('all');
    // module_invoke
}
