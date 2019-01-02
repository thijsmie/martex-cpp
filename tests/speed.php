<?php
namespace MarTeX;
require_once __DIR__ . "/tools.php";
require_once __DIR__ . "/../modules/document.php";

class TestSpeedModule extends Module
{
    public $c;
    public function __construct() {
        $this->c = \file_get_contents(__DIR__ . "/speed.bin");
    }
    public function globals() { return array("a"); }
    public function a() { return $this->c; }
    public function environments() { return array("a" => "\\MarTeX\\TestSpeedEnvironment"); }
}

class TestSpeedEnvironment extends Environment
{
    public function end($content) { return $this->module->c; }
}

start("Speed");
runstest("Spd.1", array("\\MarTeX\\TestSpeedModule"), "\\begin{a}\\end{a}");
runstest("Spd.2", array("\\MarTeX\\TestSpeedModule"), "\\a");
runstest("Spd.3", array("\\MarTeX\\DocumentModule"), \file_get_contents(__DIR__ . "/testpage.tex"));

finish();
