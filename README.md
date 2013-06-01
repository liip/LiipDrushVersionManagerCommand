# LiipDrushVersionManagerCommand

## Purpose
This drush extension implementes commands to check for new versions of installed modules and update them without issuing *drush dis module && drush pm-uninstall module*.

On update it uses a custom hook called *hook_vm_update*. With this the module can decide if there is something to do on update. And return true on success or false on failure.

It has the advantage that the *hook_install* and *hook_uninstall* is not issued. (Ex. loosing all data if the *hook_uninstall* cleans a table in the database.

## Obtain sources

### Get it from packagist.org
To obtain the sources via composer add the following lines to your composer.json file or complete the list of
dependencies.

```bash
"require": {
    "liip/drushversionmanagercommand": "1.*"
}
...
"extra": {
    "installer-paths": {
        "drush/drush/commands/{$name}/": [
            "liip/drushversionmanagercommand"
        ],
```

Then execute the following commands on the command line:

```bash
$> curl -s http://getcomposer.org/installer | php
$> php composer.phar install
```

## Getting started

Sources fetched? Brilliant.. now we can start using the module updater.
In your *ModuleName.install* file implement the *hook_install*

```php
/**
 * Update script
 */
function ModuleName_vm_update()
{
    // clean out no longer used variables
    variable_del('ModuleName_some_variable')
    
    // this function was successful
    return true;
}
```

Add a *version* to the *ModuleName.info* file:

```php
// ...
version = 1.0
// ...
```


## Let composer *post_install* or *post_update* handle module updates

```bash
#!/bin/bash
DRUSH="/bin/bash drush/drush/drush"
# store version number from installed modules
$DRUSH php-script scripts/invoke_module_update 
# update modules if needed
$DRUSH php-script scripts/update_module_versions
```

## Command line examples

### Get help
General usage:

```bash
$ /bin/bash drush/drush/drush help vm-update
```

### Update all modules
```bash
$ /bin/bash drush/drush/drush vm-update --all
Updating ModuleName1 ...                                                                                [success]
Nothing to update for module »ModuleName1«                                                              [warning]
```

### Get short list of all modules which have been updated
```bash
$ /bin/bash drush/drush/drush vm-info --all --short
ModuleName1
```

### Get full information about module update
```bash
$ /bin/bash drush/drush/drush vm-info --full --all
Title                         :  ModuleName1 
 Name                          :  ModuleName1 
 Version                       :  0.1 
 Update available to version   :  1.5
 
Title     :  ModuleName2         
 Name      :  ModuleName2         
 Version   :  7.x-2.x-dev  
 
...
```


