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

class testmodule5 extends MarTeX\Module
{
    public function globals() {return array("yay");}
    public function environments(){return array("t" => "testenv2");}
    public function yay($env, $args){
        return array(array(MarTeX\TypeString, "Nothing good!"));
    }
}

class testenv2 extends MarTeX\Environment
{
    public function end($d) { return array(array(MarTeX\TypeString, "Can't override environments,"));}
}

class testmodule6 extends MarTeX\Module
{
    public function globals() {return array("yay");}
    public function environments(){return array("t" => "testenv3");}
    public function yay($env, $args){
        return array(array(MarTeX\TypeString, "but you can override commands!"));
    }
}   

class testenv3 extends MarTeX\Environment
{
}

/// Command and Environment overrides

runmtest("M.2", array("testmodule5", "testmodule6"), "\\begin{t}\\end{t} \\yay", "Can't override environments, but you can override commands!");