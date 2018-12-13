<?php
require_once __DIR__ . "/tools.php";

use function \MarTeX\html as html;
use function \MarTeX\attr as attr;

class testmodule extends \MarTeX\Module
{
    public function globals() {return array("href");}
    public function href($env, $args){
        return html("a", attr("href", "http://www.google.com"), "google");
    }
}

runmtest("html.1", array("testmodule"), "\href", "<a href=\"http://www.google.com\">google</a>");

finish();