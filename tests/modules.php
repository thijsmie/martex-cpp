<?php

require __DIR__ . "/tools.php";

// module
class testmodule1 extends MarTeX\Module
{
    public function globals() {return array("yays1");}
    public function yays1($env, $args){
        $env->data = array(MarTeX\TypeString, "hello, ");
    }
}

class testmodule2 extends MarTeX\Module
{
    public function globals() {return array("yays2");}
    public function yays2($env, $args){
        $env->data = array(MarTeX\TypeString, "world!");
    }
}

class testmodule3 extends MarTeX\Module
{
    public function globals() {return array("yayp");}
    public function yayp($env, $args){
        return array($env->data);
    }
}

class testmodule4 extends MarTeX\Module
{
    public function environments(){return array("t" => "testenv1");}
}   

class testenv1 extends MarTeX\Environment
{
}

/// Storing variables on environments.

runmtest("M.1", array("testmodule1", "testmodule2", "testmodule3", "testmodule4"), "\\yays2\\begin{t}\\yays1\\yayp\\end{t}\\yayp", "hello, world!");

