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
            
        return 
            html("i", bytes($args[0])) .
            mstr(": ") .
            html("b", bytes($args[1])) .
            newline() .
            html("p", bytes($args[2]))
        ;
    }

    public function envdescriptor($env, $args) 
    {
        if (count($args) !== 2)
            throw new \Exception("Envdescriptor takes 2 arguments.");
            
        return batch(
            html("i", bytes($args[0])),
            html("br"), 
            html("p", bytes($args[1]))
        );
    }

    public function command($env, $args)
    {
        $cmd = mstr("&#92;") . bytes($args[0]);

        if (count($args) == 1)
            return $cmd;

        for($i=1; $i<count($args); $i++)
            $cmd .= mstr("&#123;") . bytes($args[$i]) . mstr("&#125;");

        return $cmd;
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

        $a = html("!DOCTYPE html",
            html("html",
                html("head", $head) .
                $body
            )
        );

        file_put_contents(__DIR__ . "/../tests/speed.bin", $a);

        return $a;
    }
}

class BodyEnvironment extends Environment
{
    public function end($content) 
    {
        return html("body", bytes($content));
    }
}