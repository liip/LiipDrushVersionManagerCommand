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
            return dt('Run an update callback function of a module or all installed modules.');
    }
}

/**
 * Implementation of the drush command hook.
 *
 * Defines the commands added by this extension.
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
 * @param string $modules Comma delimited list of modules to be updated
 */
function drush_vm_information($modules)
{
    if (empty($modules) && _drush_vm_hasOption('all')) {

        // get all installed modules
        $modules = module_list();

    } else {

        $modules = _drush_vm_get_modules_from_arguments($modules);

        if (empty($modules)) {
            return drush_set_error(
                'EMPTY_ARGUMENT_LIST',
                'List of modules or option "--all" missing.',
                'Version Manager Command Error'
            );
        }
    }

    $extensionInfo = drush_get_extensions(false);

    foreach ($modules as $module) {

        if (isset($extensionInfo[$module])) {
          $info = $extensionInfo[$module];

        } else {
          drush_log(dt('!extension was not found.', array('!extension' => $module)), 'warning');
          continue;
        }

        if ($info->type != 'module') {
            drush_log(dt('!extension is not module.', array('!extension' => $module)), 'warning');
            continue;
        }

        $data['Title'] = $info->info['name'];
        $data['Name'] = $info->name;
        $data['Version'] = $info->info['version'];
        $data['Update available to version'] = '5555';

        drush_print_table(drush_key_value_to_array_table($data));
        print "\n";
    }
}

/**
 * Runs the update of each specified module.
 *
 * @param string $modules Comma delimited list of modules to be updated
 */
function drush_vm_run($modules)
{
    // drush_get_option('all');
    // module_invoke

    drush_print('Not implemented yet ... stay tuned!');
}

/**
 * Determines whether an option is set or not.
 *
 * @param string $option
 * @param bool $default
 * @param string $context
 *
 * @return null
 */
function _drush_vm_hasOption($option, $default = false, $context = null)
{
    return drush_get_option($option, $default, $context);
}

/**
 * Split the provides csv into a list.
 *
 * @param string $modules Comma delimited list of module names.
 *
 * @return array
 */
function _drush_vm_get_modules_from_arguments($modules)
{
    $pattern = "@,\s*@";
    return preg_split($pattern, $modules, -1, PREG_SPLIT_NO_EMPTY);
}
