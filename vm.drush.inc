<?php
/**
 * @file Drush Module Version Management commands
 */

/**
 *
 */
function vm_drush_help($section)
{
    switch ($section) {
        case 'meta:vm:title':
            return dt('Module Version commands');
        case 'meta:vm:summary':
            return dt('Run an update callback function of a module or all installed modules.');
        case 'error:EMPTY_ARGUMENT_LIST':
            return dt(
                'List of modules or option "--all" missing. ' .
                'Execute »drush help vm-info« for detailed information'
            );
        case 'error:VERSION_FILE_NOT_FOUND':
            return dt(
                'Version file not available. Please make sure it is available in directory: ' . DRUPAL_ROOT
            );
    }
}

/**
 * Implementation of the drush command hook.
 * Defines the commands added by this extension.
 */
function vm_drush_command()
{
    $items['versionmanager-information'] = array(
        'callback'           => 'drush_vm_information',
        'description'        => 'Print information about the specified module(s).',
        'aliases'            => array('vm-info'),
        'examples'           => array(
            'drush versionmanager-information --all' =>
            'Display information about any provided module if there is an update available.',
        ),
        'arguments'          => array(
            'modules' => 'A comma delimited list of module names',
        ),
        'required-arguments' => false,
        'options'            => array(
            'all'   => 'Run the update on all installed and enabled modules',
            'full'  => 'show extended information about the module (this is the default)',
            'short' => 'show basic information about the module',
        ),
    );

    $items['versionmanager-update'] = array(
        'callback'           => 'drush_vm_run',
        'description'        => 'Print information about the specified module(s).',
        'aliases'            => array('vm-update'),
        'examples'           => array(
            'drush versionmanager-update --all' =>
            'Update every named module to the new version.',
        ),
        'arguments'          => array(
            'modules' => 'A comma delimited list of module names',
        ),
        'required-arguments' => false,
        'options'            => array(
            'all' => 'Run the update on all installed and enabled modules where an update is available.',
        ),
    );

    return $items;
}

/**
 * Prints information about the specified module(s).
 *
 * @param string $modules Comma delimited list of modules to be updated
 */
function drush_vm_information($modules = '')
{
    try {
        $modules = _drush_vm_determine_module_list($modules);
    } catch (InvalidArgumentException $e) {
        return drush_set_error($e->getMessage());
    }

    $extensionInfo = drush_get_extensions(false);

    foreach ($modules as $module) {

        try {
            $info = _drush_vm_get_module_info($module, $extensionInfo);

        } catch (InvalidArgumentException $e) {
            drush_log($e->getMessage(), 'warning');
            continue;
        }

        if (_drush_vm_hasOption('short')) {

            if (-1 < ($version = _drush_vm_determine_updateable($info))) {
                $data[] = $info->name;
            }

        } else {
            $data = _drush_vm_transcode_to_schema($info);
            drush_print_table(drush_key_value_to_array_table($data));
        }
    }

    if (_drush_vm_hasOption('short')) {
        drush_print(implode(',', $data));
    }
}

/**
 * Runs the update of each specified module.
 *
 * @param string $modules Comma delimited list of modules to be updated
 */
function drush_vm_run($modules = '')
{
    try {
        $modules = _drush_vm_determine_module_list($modules);
    } catch (InvalidArgumentException $e) {
        return drush_set_error($e->getMessage());
    }

    $extensionInfo = drush_get_extensions(false);

    foreach ($modules as $module) {

        try {
            $info = _drush_vm_get_module_info($module, $extensionInfo);

        } catch (InvalidArgumentException $e) {
            drush_log($e->getMessage(), 'warning');
            continue;
        }

        if (-1 === _drush_vm_determine_updateable($info)) {
            drush_log(dt('Nothing to update for module »!module«', array('!module' => $module)), 'warning');
            continue;
        }

        // load module install file
        module_load_include('install', $module, $module);

        if (!module_hook($module, 'vm_update')) {
            drush_log(
                dt('Updating !module ... no »vm_update« hook available', array('!module' => $module)),
                'warning'
            );
            continue;
        }

        $lastVersion = _drush_vm_read_version_from_file($info->name);
        $currentVersion = $info->info['version'];

        if (!module_invoke($module, 'vm_update', array($lastVersion, $currentVersion))) {

            drush_log(dt('Updating !module ...', array('!module' => $module)), 'error');
            continue;
        }
        drush_log(dt('Updating !module ...', array('!module' => $module)), 'success');
    }

}

/**
 * Finds information about the given module.
 *
 * @param string $moduleName
 * @param array $extensionInfo
 *
 * @return array
 * @throws InvalidArgumentException
 */
function _drush_vm_get_module_info($moduleName, array $extensionInfo)
{
    if (isset($extensionInfo[$moduleName])) {
        $info = $extensionInfo[$moduleName];
    } else {
        throw new InvalidArgumentException(
            dt('Module »!extension« was not found.', array('!extension' => $moduleName))
        );
    }

    if ($info->type != 'module') {
        throw new InvalidArgumentException(
            dt('»!extension« is not module.', array('!extension' => $moduleName))
        );
    }

    return $info;
}

/**
 * Determines what module either to be checked for an update or to be updated.
 *
 * @param string $modules
 *
 * @return array
 * @throws InvalidArgumentException in case no module was found.
 */
function _drush_vm_determine_module_list($modules = '')
{
    if (empty($modules) && _drush_vm_hasOption('all')) {

        $modules = module_list();
    } else {
        $modules = _drush_vm_get_modules_from_arguments($modules);

    }

    if (empty($modules)) {
        throw new InvalidArgumentException('EMPTY_ARGUMENT_LIST');
    }

    return $modules;
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

/**
 * Transcode module information to a schema usable for a cli output.
 *
 * @param array $info
 *
 * @return array
 */
function _drush_vm_transcode_to_schema(array $info)
{
    $data = array();
    $data['Title'] = $info->info['name'];
    $data['Name'] = $info->name;

    $version = _drush_vm_read_version_from_file($info->name);
    $data['Version'] = (-1 < $version) ? $version : dt('no version information found.');

    if (-1 < ($version = _drush_vm_determine_updateable($info))) {
        $data['Update available to version'] = $version;
    }

    return $data;
}

/**
 * Determines if there is a new version available.
 *
 * @param array $info
 *
 * @return string
 */
function _drush_vm_determine_updateable(array $info)
{
    $lastVersion = _drush_vm_read_version_from_file($info->name);
    $currentVersion = $info->info['version'];

    if (version_compare($lastVersion, $currentVersion, "<")) {
        return $currentVersion;
    }

    return -1;
}

/**
 * Reads the current version of the provides module from a file.
 *
 * @param string $moduleName
 * @param string $filename
 *
 * @return string
 */
function _drush_vm_read_version_from_file($moduleName, $filename = null)
{
    if (is_null($filename)) {
        $filename = DRUPAL_ROOT . "/versions.json";
    }

    if (!file_exists($filename)) {
        return drush_set_error('VERSION_FILE_NOT_FOUND');
    }

    $data = json_decode(file_get_contents($filename), true);

    if (!empty($data[$moduleName])) {
        return $data[$moduleName];
    }

    return -1;
}
