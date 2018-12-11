<?php
namespace TEST_HTML;
require_once __DIR__ . "/tools.php";

class testmodule extends \MarTeX\Module
{
    public function globals() {return array("href");}
    public function href($env, $args){
        return array(array("a", array(array("href", "http://www.google.com"), array(\MarTeX\TypeString, "google"))));
    }
}

runmtest("html.1", array("\\TEST_HTML\\testmodule"), "\href", "<a href=\"http://www.google.com\">google</a>");

finish();