<?php

require_once __DIR__ . "/tools.php";
require_once __DIR__ . "/../modules/document.php";

$mods = array();

start("Document");

runmtest("D.1", $mods, "\\begin{paragraph}hoi\\end{paragraph}", "<p>hoi</p>");
runmtest("D.2", $mods, "\\colour{00ff00}{hoi}", "<span style=\"color: #00ff00\">hoi</span>");
runmtest("D.3", $mods, "\\color{00ff00}{hoi}", "<span style=\"color: #00ff00\">hoi</span>");
runmtest("D.6", $mods, "\\href{a.com}{b}", "<a href=\"a.com\">b</a>");

finish();