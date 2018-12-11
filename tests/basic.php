<?php
namespace TEST_BASIC;

require_once __DIR__ . "/tools.php";

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
    public function environments(){return array("test"=>"\\TEST_BASIC\\test");}
    public function yay($env, $args){
        array_push($args[0], array(\MarTeX\TypeString, " yay"));
        return $args[0];
    }
}
class test extends \MarTeX\Environment
{
    public function locals() {return array("yay");}
    public function yay($env, $args){
        array_push($args[0], array(\MarTeX\TypeString, "yay"));
        return $args[0];
    }
    public function begin($b) {}
    public function end($d) { return $d;}
}

runmtest("E.1", array("\\TEST_BASIC\\testmodule"), "\\begin{test}test\\end{test}", "test");
runmtest("E.2", array("\\TEST_BASIC\\testmodule"), "\\yay{1} \\begin{test}\\yay{2}\\end{test}", "1 yay 2yay");

// STDLib

runtest("std.1", "\\Alpha\\alpha", "&Alpha;&alpha;");
runtest("std.2", "\\section{test}", "<h3>test</h3>");
runtest("std.3", "\\euro\\euro\\euro a \\subsection{Eyyyy}", "&euro;&euro;&euro;a <h4>Eyyyy</h4>");


// Special chars

runtest("esc.1", "{", "");
runtest("esc.2", "<", "");
runtest("esc.3", "\{", "&#123;");
runtest("esc.4", "\<", "&lt;");
runtest("esc.5", "\\\"", "");
runtest("esc.6", "\\\"e", "&euml;");
runtest("esc.7", "\\hline", "<hr>");

// Whitespace

runtest("ws.1", "\\euro a", "&euro;a");
runtest("ws.2", "\\euro\ a", "&euro; a");
runtest("ws.3", "\\euro\\euro\\ a", "&euro;&euro; a");
runtest("ws.4", "\\'ea", "&eacute;a");

finish();