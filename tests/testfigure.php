<?php

require_once __DIR__ . "/tools.php";
require_once __DIR__ . "/../modules/figure.php";

$mods = array("\\MarTeX\\FigureModule");


runmtest("F.1", $mods, "\\figure_header{Figure}", "");
runmtest("F.2", $mods, 
    "\\begin{figure}\\includegraphics{y}\\end{figure}", 
    "<img width=\"100%\" alt=\"\" href=\"y\"/>");
runmtest("F.3", $mods, 
    "\\begin{figure}\\includegraphics{y}\\caption{hey!}\\end{figure}", 
    "<figure><img width=\"100%\" alt=\"\" href=\"y\"/><figcaption>Figure 1: hey!</figcaption></figure>");
runmtest("F.4", $mods, 
    "\\figure_origin{/a/}\\begin{figure}\\includegraphics{y}\\end{figure}", 
    "<img width=\"100%\" alt=\"\" href=\"/a/y\"/>");
runmtest("F.5", $mods, 
    "\\figure_header{Fogure}\\begin{figure}\\includegraphics{y}\\caption{hey!}\\end{figure}", 
    "<figure><img width=\"100%\" alt=\"\" href=\"y\"/><figcaption>Fogure 1: hey!</figcaption></figure>");
runmtest("F.6", $mods, 
    "\\begin{figure}\n\\width{80}\\includegraphics{y}\\end{figure}", 
    "<img width=\"80%\" alt=\"\" href=\"y\"/>");
runmtest("F.7", $mods, 
    "\\begin{figure}\n\\width{80px}\\includegraphics{y}\\end{figure}", 
    "<img width=\"80px\" alt=\"\" href=\"y\"/>");
finish();