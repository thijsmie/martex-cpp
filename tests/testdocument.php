<?php

require_once __DIR__ . "/tools.php";
require_once __DIR__ . "/../modules/document.php";

$mods = array("\\MarTeX\\DocumentModule");

start("Document");

runmtest("D.1", $mods, "\\begin{paragraph}hoi\\end{paragraph}", "<p>hoi</p>");
runmtest("D.2", $mods, "\\colour{#00ff00}{hoi}", "<span style=\"color:#00ff00\">hoi</span>");
runmtest("D.2", $mods, "\\colour{34,55,88,5}{hoi}", "<span style=\"color:rgba(34,55,88,5)\">hoi</span>");
runmtest("D.3", $mods, "\\color{#00ff00}{hoi}", "<span style=\"color:#00ff00\">hoi</span>");
runmtest("D.4", $mods, "\\colour{hsl}{0,20,20}{hoi}", "<span style=\"color:hsl(0,20%,20%)\">hoi</span>");
runmtest("D.5", $mods, "\\colour{hsl}{0,20,20}{hoi \\textbf{hoi}}", "<span style=\"color:hsl(0,20%,20%)\">hoi <b>hoi</b></span>");

finish();