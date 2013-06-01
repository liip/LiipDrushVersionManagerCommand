<?php

class VmTest extends PHPUnit_Framework_TestCase
{
    /**
     * @dataProvider getModulesFromArgumentsDataprovider
     */
    public function testGetModulesFromArguments($modules, $expected)
    {
        $this->assertEquals($expected, _drush_vm_get_modules_from_arguments($modules));
    }
    public static function getModulesFromArgumentsDataprovider()
    {
        return array(
            'no modules (just commas) provided' => array(',,,', array()),
            'no modules provided' => array('', array()),
            'one module provided' => array('tux', array('tux')),
            'multiple modules provided' => array('tux, devil', array('tux', 'devil')),
        );
    }

}
