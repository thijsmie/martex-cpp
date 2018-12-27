<?php

namespace MarTeX;

class DocumentationModule extends Module
{
    public function globals() { return array("descriptor", "envdescriptor", "command"); }
    public function environments() { 
        return array(
            "page" => "\\MarTeX\\PageEnvironment", 
            "document" => "\\MarTeX\\BodyEnvironment"
        ); 
    }

    public function descriptor($env, $args) 
    {
        if (count($args) !== 3)
            throw new \Exception("Descriptor takes 3 arguments.");
            
        array_push($args[0], value(TypeString, ": "));
        return batch(
            html("i", $args[0]), 
            html("b", $args[1]), 
            value(TypeBreak, ""),
            html("p", $args[2])
        );
    }

    public function envdescriptor($env, $args) 
    {
        if (count($args) !== 2)
            throw new \Exception("Envdescriptor takes 2 arguments.");
            
        return batch(
            html("i", $args[0]),
            html("br", value(TypeNull, "")), 
            html("p", $args[1])
        );
    }

    public function command($env, $args)
    {
        $cmd = array(value(TypeString, "&#92;"), $args[0]);

        if (count($args) == 1)
            return batch($cmd);

        for($i=1; $i<count($args); $i++)
            array_push($cmd, batch(value(TypeString, "&#123;"), $args[$i], value(TypeString, "&#125;")));

        return batch($cmd);
    }
}

class PageEnvironment extends Environment
{
    public function end($content) 
    {
        $body = null;
        $head = array();

        foreach($content as $item) {
            if ($item[0] == TypeHtml && $item[1] == "body") {
                if ($body !== null)
                    throw new \Exception("Two bodies is not allowed!");
                $body = $item;
            }
            else {
                array_push($head, $item);
            }
        }

        $a = html("!DOCTYPE html",
            html("html",
                html("head", batch($head)),
                $body
            )
        );
        return $a;
    }
}

class BodyEnvironment extends Environment
{
    public function end($content) 
    {
        return html("body", $content);
    }
}