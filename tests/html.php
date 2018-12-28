<?php
require_once __DIR__ . "/tools.php";

use function \MarTeX\html as html;
use function \MarTeX\attr as attr;
use function \MarTeX\mstr as mstr;

class testmodule extends \MarTeX\Module
{
    public function globals() {return array("href1", "href2", "href3", "href4");}
    public function href1($env, $args){
        return html("a", attr("href", "http://www.google.com"));
    }
    public function href2($env, $args){
        return html("a", attr("href", "http://www.google.com") . mstr("google"));
    }
    public function href3($env, $args){
        return html("a", attr("href", "http://www.google.com"));
    }
    public function href4($env, $args){
        return html("a", attr("href", "http://www.google.com") . mstr("google"));
    }

}

start("Html");

runmtest("html.1", array("testmodule"), "\href1", "<a href=\"http://www.google.com\"/>");
runmtest("html.2", array("testmodule"), "\href2", "<a href=\"http://www.google.com\">google</a>");
runmtest("html.3", array("testmodule"), "\href3", "<a href=\"http://www.google.com\"/>");
runmtest("html.4", array("testmodule"), "\href4", "<a href=\"http://www.google.com\">google</a>");

finish();