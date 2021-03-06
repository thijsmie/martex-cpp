<?php

require_once __DIR__ . "/tools.php";

start("Basic");

// input=output
runtest("io.1", "a", "a");

// ampersands
runtest("&.1", "&", "&amp;");
runtest("&.2", "&&", "&amp;&amp;");
runtest("&.1", "\&", "&amp;");
runtest("&.2", "\&\&", "&amp;&amp;");

// module
class testmodule extends \MarTeX\Module
{
    public function globals() {return array("yay");}
    public function environments(){return array("test"=>"test");}
    public function yay($env, $args){
        return \MarTeX\mstr($args[0][0][1] . " yay");
    }
}
class test extends \MarTeX\Environment
{
    public function locals() {return array("yay");}
    public function yay($env, $args){
        return \MarTeX\mstr($args[0][0][1] . "yay");
    }
}

runmtest("E.1", array("testmodule"), "\\begin{test}test\\end{test}", "test");
runmtest("E.2", array("testmodule"), "\\yay{1} \\begin{test}\\yay{2}\\end{test}", "1 yay 2yay");

// STDLib

runtest("std.1", "\\Alpha\\alpha", "&Alpha;&alpha;");
runtest("std.2", "\\section{test}", "<h3>test</h3>");
runtest("std.3", "\\euro\\euro\\euro a \\subsection{Eyyyy}", "&euro;&euro;&euro;a <h4>Eyyyy</h4>");


// Special chars

runtest("esc.1", "{", "", true);
runtest("esc.2", "<", "", true);
runtest("esc.3", "\{", "&#123;");
runtest("esc.4", "\<", "&lt;");
runtest("esc.5", "\\\"", "", true);
runtest("esc.6", "\\\"e", "&euml;");
runtest("esc.7", "\\hline", "<hr>");

// Whitespace

runtest("ws.1", "\\euro a", "&euro;a");
runtest("ws.2", "\\euro\ a", "&euro; a");
runtest("ws.3", "\\euro\\euro\\ a", "&euro;&euro; a");
runtest("ws.4", "\\'ea", "&eacute;a");

// Variable

runtest("var.1", "\\define{a}{b}#a", "b");

finish();