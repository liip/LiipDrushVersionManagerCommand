# LiipDrushVersionManagerCommand
## Purpose
This drush extension implementes commands to check for new versions of installed modules and update them without issuing *drush dis module && drush pm-uninstall module*.
On update it uses a custom hook called `hook_vm_update()`. With this the module can decide if there is something to do on update. And return true on success or false on failure.
It has the advantage that the `hook_install()` and `hook_uninstall()` is not issued. (E.g. loosing all data if the `hook_uninstall()` cleans a table in the database.

## Travis build status
[![Build Status](https://travis-ci.org/liip/LiipDrushVersionManagerCommand.png?branch=master)](https://travis-ci.org/liip/LiipDrushVersionManagerCommand)

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
        "path/2/drush/commands/{$name}/": [
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
In your *ModuleName.install* file implement the *hook_vm_update()*.

```php
/**
 * Update script
 *
 * @return bool
 */
function ModuleName_vm_update()
{
    // clean out no longer used variables
    variable_set('ModuleName_some_variable', 'myValue')
    
    // this update was successful
    return (bool) variable_get('ModuleName_some_variable', false);
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
DRUSH="drush"

# store version number from installed modules
$DRUSH php-script scripts/invoke_module_update 

# update modules if needed
$DRUSH php-script scripts/update_module_versions
```

## Command line examples

### Get help
General usage:

```bash
$ drush help vm-update
Print information about the specified module(s).

Examples:
 drush versionmanager-update --all   Update every named module to the new version. 


Arguments:
 modules                             A comma delimited list of module names 
                                     (e.g. module1,module2,module2) 

Options:
 --all                               Run the update on all installed and enabled modules where an 
                                     update is available. 

Aliases: vm-update
```

```bash
$ drush help vm-info
Print information about the specified module(s).

Examples:
 drush versionmanager-information --all    Display information about any provided module if there 
                                           is an update available. 

Arguments:
 modules                                   A comma delimited list of module names 


Options:
 --all                                     Run the update on all installed and enabled modules              
 --full                                    show extended information about the module (this is the default) 
 --short                                   show basic information about the module                          


Aliases: vm-info
```

### Update all modules
```bash
$ drush vm-update --all
Updating ModuleName1 ...                                                                   [success]
Nothing to update for module »ModuleName2«                                                 [warning]
```

### Update a subset of all enabled modules
```bash
$ drush vm-update ModuleName1, ModuleName2
Updating ModuleName1 ...                                                                   [success]
Updating ModuleName2 ...                                                                   [success]
```

### Get short list of all modules were an update is available
The neat thing is that you can copy and paste the out put directly to `vm-update` and not using the *--all* switch there. This will make it segnificantly faster.

```bash
$ drush vm-info --all --short
ModuleName1, ModuleName2, ..., ModuleNameN
```

### Get full information about module updates
The *--full* switch is not only optional but also the default if *--short* is not declared. As you'd guess ModuleName2 has no update available.

```bash
$ drush vm-info --full --all
Title                         :  Title of ModuleName1
Name                          :  ModuleName1 
Version                       :  0.1 
Update available to version   :  1.5
 
Title     :  Title of ModuleName2
Name      :  ModuleName2 
Version   :  7.x-2.x-dev  
 
...
```
