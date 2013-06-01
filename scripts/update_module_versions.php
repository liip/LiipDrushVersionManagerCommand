<?php

# config: name of the versions file
$fileName = DRUPAL_ROOT . "/versions.json";

# get all module informations
$modules = drush_get_extensions(false);

# get all versions from enabled modules
$versions = array();
foreach ($modules as $name => $module) {
    if ('enabled' === drush_get_extension_status($module)) {
        $versions[$name] = $module->info['version'];
    }
}

# write version to disc
file_put_contents($fileName, json_encode($versions));
