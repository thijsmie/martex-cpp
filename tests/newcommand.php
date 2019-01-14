<?php

require_once __DIR__ . "/tools.php";

start("Newcommand");

// input=output
runtest("nc.1", 
    "\\newcommand\\bla{c}\\bla", 
    "c"
);
runtest("nc.2", 
    "\\newcommand\\1{\\2\\2\\2\\2}\\newcommand\\2{\\3\\3\\3\\3}\\newcommand\\3{abcd}\\1\\1\\1\\1", 
    "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd"
);
runtest("nc.2", 
    "\\newcommand\\1{\\1}\\1", 
    "", true);
runtest("nc.3", 
    "\\newcommand\\1[1]{#1}\\1{2}", 
    "2");

finish();