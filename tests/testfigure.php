<?php

require_once __DIR__ . "/tools.php";
$mods = array();

start("Figure");
runmtest("F.1", $mods, 
    "\\begin{figure}\\includegraphics{y}\\end{figure}", 
    "<img width=\"100%\" alt=\"\" href=\"y\"/>");
runmtest("F.2", $mods, 
    "\\begin{figure}\\includegraphics{y}\\caption{hey!}\\end{figure}", 
    "<figure><img width=\"100%\" alt=\"\" href=\"y\"/><figcaption>Figure 1: hey!</figcaption></figure>");
runmtest("F.3", $mods, 
    "\\begin{figure}\\includegraphics{y.png}\\end{figure}", 
    "<img width=\"100%\" alt=\"\" href=\"y.png\"/>");
runmtest("F.4", $mods, 
    "\\begin{figure}\n\\width{80}\\includegraphics{y}\\end{figure}", 
    "<img width=\"80%\" alt=\"\" href=\"y\"/>");
runmtest("F.5", $mods, 
    "\\begin{figure}\n\\width{80px}\\includegraphics{y}\\end{figure}", 
    "<img width=\"80px\" alt=\"\" href=\"y\"/>");
runmtest("F.6", $mods, 
    "\\begin{figure}\\define{header}{Fogure }\\includegraphics{y}\\caption{hey!}\\end{figure}", 
    "<figure><img width=\"100%\" alt=\"\" href=\"y\"/><figcaption>Fogure 1: hey!</figcaption></figure>");
finish();