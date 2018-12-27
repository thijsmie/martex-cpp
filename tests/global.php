<?php

require_once __DIR__ . "/tools.php";

class testmodule1 extends \MarTeX\Module
{
    public function environments() {return array("a" => "testenv1", "b" => "testenv2");}
}

class testenv1 extends \MarTeX\Environment
{
    public function locals() { return array("a"); }
    public function a($env, $args) { $this->tex->a = \MarTeX\value(\MarTeX\TypeString, "c"); }
}

class testenv2 extends \MarTeX\Environment
{
    public function locals() { return array("b"); }
    public function b($env, $args) { return $this->tex->a; }
}

start("Global");

/// Storing variables on global.
runmtest("G.1", array("testmodule1"), 
         "\\begin{a}\\a\\end{a}\\begin{b}\\b\\end{b}", "c");
runmtest("G.2", array("testmodule1"), 
         "\\section{Hallo}\\label{a}\\begin{b}\\b\\end{b}", "<h3>Hallo</h3>1");
finish();