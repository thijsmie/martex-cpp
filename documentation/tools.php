<?php

namespace MarTeX;

class DocumentationModule extends Module
{
    public function globals() { return array("descriptor", "envdescriptor", "command"); }
    public function environments() { 
        return array(
            "page" => "\\MarTeX\\PageEnvironment", 
            "document" => "\\MarTeX\\BodyEnvironment",
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
        return batch(
            html("i", $args[0]),
            html("br"), 
            html("p", $args[1])
        );
    }

    public $argsfor_command = array(Bytes, MoreBytes);
    public function command($env, $args)
    {
        $cmd = mstr("&#92;") . $args[0];

        if (count($args) == 1)
            return $cmd;

        for($i=1; $i<count($args); $i++)
            $cmd .= mstr("&#123;") . $args[$i] . mstr("&#125;");

        return $cmd;
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

class PageEnvironment extends Environment
{
    public function end($content) 
    {
        $body = null;
        $head = "";

        foreach($content as $item) {
            if ($item[0] == TypeHtml && $item[1] == "body") {
                if ($body !== null)
                    throw new \Exception("Two bodies is not allowed!");
                $body = bytes($item);
            }
            else {
                $head .= bytes($item);
            }
        }

        $a = mstr("<!DOCTYPE html>") .
            html("html",
                html("head", $head) .
                $body
            );

        //file_put_contents(__DIR__ . "/../tests/speed.bin", $a);

        return $a;
    }
}

class BodyEnvironment extends Environment
{
    public $argsfor_end = array(Bytes);
    public function end($content) 
    {
        return html("body", $content);
    }
}