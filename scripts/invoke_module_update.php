<?php

# config: name of the versions file
$fileName = DRUPAL_ROOT . "/versions.json";

# get previous installed versions
if (file_exists($fileName)) {
    $installedVersion = json_decode(file_get_contents($fileName), true);

    # get all module informations
    $modules = drush_get_extensions(false);

    # get all versions from enabled modules
    foreach ($modules as $name => $module) {
        if ('enabled' === drush_get_extension_status($module)) {
            if (isset($installedVersion[$name]) && version_compare($installedVersion[$name], $module->info['version'], '<')) {
                module_invoke($name, 'vm_update');    
                drush_log("Updated module: " . $name, 'success');
            }
        }
    }
} else {
    drush_log("No previous installed modules found: skipping!" . $name, 'warning');
}

