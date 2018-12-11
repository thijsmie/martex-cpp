<?php

require __DIR__ . "/tools.php";

// module
class testmodule1
{
    public function globals() {return array("yays1");}
    public function environments(){return array();}
    public function yays1($env, $args){
        $env->data = array(MarTeX\TypeString, "hello, ");
    }
}

class testmodule2
{
    public function globals() {return array("yays2");}
    public function environments(){return array();}
    public function yays2($env, $args){
        $env->data = array(MarTeX\TypeString, "world!");
    }
}

class testmodule3
{
    public function globals() {return array("yayp");}
    public function environments(){return array();}
    public function yayp($env, $args){
        return array($env->data);
    }
}

class testmodule4
{
    public function globals() {return array();}
    public function environments(){return array("t" => "testenv1");}
}

class testenv1 extends MarTeX\Environment
{
    public function locals() {return array();}
    public function begin($b, $c) {}
    public function end($d) { return $d;}
}

/// Storing variables on environments.

runmtest("M.1", array("testmodule1", "testmodule2", "testmodule3", "testmodule4"), "\\yays2\\begin{t}\\yays1\\yayp\\end{t}\\yayp", "hello, world!");

