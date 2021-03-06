<?php

require_once __DIR__ . "/tools.php";

$mods = array();

start("Itemize");
runmtest("I.1", $mods,
    "\\begin{itemize} \\end{itemize}",
    "<ul></ul>"
);
runmtest("I.2", $mods,
    "\\begin{enumerate} \\end{enumerate}",
    "<ol></ol>"
);
runmtest("I.3", $mods,
    "\\begin{itemize} \\item foo \\item bar\\end{itemize}",
    "<ul><li>foo </li><li>bar</li></ul>"
);
runmtest("I.4", $mods,
    "\\begin{itemize} \\item foo \\item bar \\setmarker{ROMAN} \\end{itemize}",
    "<ol type=\"I\"><li>foo </li><li>bar  </li></ol>"
);
runmtest("I.5", $mods,
    "\\begin{itemize} \\item{foo }\\item bar \\item{baz} \\end{itemize}",
    "<ul><li>foo </li><li>bar </li><li>baz</li></ul>"
);
finish();