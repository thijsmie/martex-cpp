<?php

require_once __DIR__ . "/tools.php";
require_once __DIR__ . "/../modules/itemize.php";

$mods = array("\\MarTeX\\ItemizeModule");

runmtest("I.1", $mods,
    "\\begin{itemize} \\end{itemize}",
    "<ul/>"
);
runmtest("I.2", $mods,
    "\\begin{enumerate} \\end{enumerate}",
    "<ol/>"
);