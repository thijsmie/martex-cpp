<?php

require_once __DIR__ . "/tools.php";
require_once __DIR__ . "/../modules/itemize.php";

$mods = array("\\MarTeX\\ItemizeModule");


start("Itemize");
runmtest("I.1", $mods,
    "\\begin{itemize} \\end{itemize}",
    "<ul/>"
);
runmtest("I.2", $mods,
    "\\begin{enumerate} \\end{enumerate}",
    "<ol/>"
);
runmtest("I.3", $mods,
    "\\begin{itemize} \\item foo \\item bar\\end{itemize}",
    "<ul><li>foo </li><li>bar</li></ul>"
);
runmtest("I.3", $mods,
    "\\begin{itemize} \\item foo \\item bar \\setmarker{ROMAN} \\end{itemize}",
    "<ol type=\"I\"><li>foo </li><li>bar  </li></ol>"
);
finish();