<?php

namespace MarTeX;

class DocumentationModule extends Module
{
    public function globals() { return array("descriptor", "envdescriptor"); }
    public function environments() { 
        return array(
            "code" => "\\MarTeX\\CodeEnvironment"
        ); 
    }

    public $argsfor_descriptor = array(Bytes, Bytes, Bytes);
    public function descriptor($env, $args) 
    {
        return 
            html("i", $args[0]) .
            mstr(": ") .
            html("b", $args[1]) .
            newline() .
            html("p", $args[2])
        ;
    }

    public $argsfor_envdescriptor = array(Bytes, Bytes);
    public function envdescriptor($env, $args) 
    {
        return 
            html("i", $args[0]) .
            newline() .
            html("p", $args[1]);
    }
}

class CodeEnvironment extends Environment
{
    private $language = "latex";

    public $argsfor_begin = PlainText;
    public function begin($bracket_argument)
    {
        if ($bracket_argument !== "")
            $this->language = $bracket_argument;
    }

    public $argsfor_end = array(Bytes);
    public function end($content)
    {
        return html("pre",
            html("code", 
                attr("class", "language-" . $this->language) .
                $content
            )
        );
    }
}