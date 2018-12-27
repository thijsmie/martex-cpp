<?php

require_once __DIR__ . "/tools.php";

start("Labels");

runtest("l.1", "\\chapter{a}\\label{chp:a}\\ref{chp:a}", "<h2>a</h2>1");
runtest("l.2", "\\section{a}\\label{sec:a}\\ref{sec:a}", "<h3>a</h3>1");
runtest("l.3", "\\section{a}\\subsection{b}\\label{sub:b}\\ref{sub:b}", "<h3>a</h3><h4>b</h4>1.1");

finish();