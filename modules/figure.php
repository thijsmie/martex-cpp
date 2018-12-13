<?php
namespace MarTeX;

class FigureModule extends Module {       
    private $labelNumber;
    private $labelPrefix;
    private $origin;

    public function __construct() {
        $this->labelNumber = 1;
        $this->labelPrefix = "Figure";
        $this->origin = "";
    }

    public function globals() {
        return array("figure_header", "figure_origin");
    }

    public function figure_header($env, $args)
    {
        if (count($args) != 1 || !is_string($args[0][0]))
            throw new \Exception("Invalid arguments");
        $this->labelPrefix = $args[0][0];
    }

    public function figure_origin($env, $args)
    {
        if (count($args) != 1 || !is_string($args[0][0]))
            throw new \Exception("Invalid arguments");
        $this->origin = $args[0][0];
    }

    public function environments() {
        return array("figure"=>"\\MarTeX\\FigureEnvironment");
    }
    
    public function getLabel() {
        return $this->labelPrefix . " " . $this->labelNumber++;
    }

    public function getOrigin() {
        return $this->origin;
    }
}

class FigureEnvironment extends Environment
{
    private $_alttext = "", $_caption;
    private $_width, $_height, $_label, $_image;

    public function locals() 
    {
        return array("caption", "includegraphics", "width", 
                     "height", "alttext");
    }

    public function begin($bracket)
    {
        $this->_label = $this->module->getLabel();
    }

    public function width($env, $args)
    {
        if (count($args) != 1 || !is_string($args[0][0]))
            throw new \Exception("Invalid arguments");
        $this->_width = $args[0][0];
    }

    public function height($env, $args)
    {
        if (count($args) != 1 || !is_string($args[0][0]))
            throw new \Exception("Invalid arguments");
        $this->_height = $args[0][0];
    }

    public function caption($env, $args)
    {
        if (count($args) != 1 || !is_string($args[0][0]))
            throw new \Exception("Invalid arguments");
        $this->_caption = $args[0][0];
    }

    public function includegraphics($env, $args)
    {
        if (count($args) != 1 || !is_string($args[0][0]))
            throw new \Exception("Invalid arguments");
        $this->_image = $this->module->getOrigin() . $args[0][0];
    }

    public function alttext($env, $args)
    {
        if (count($args) != 1 || !is_string($args[0][0]))
            throw new \Exception("Invalid arguments");
        $this->_alttext = $args[0][0];
    }

    public function end($content) {    
        if (!isset($this->_image))
            throw \Exception("Figure did not contain an image!");

        /// Attributes
        $attributes = array();
        if (!isset($this->_width) && !isset($this->_height)) {
            array_push($attributes, attr("width", "100%"));
        }

        if(isset($this->_width)) {
            if (ctype_digit($this->_width))
                array_push($attributes, attr("width", $this->_width . "%"));
            else
                array_push($attributes, attr("width", $this->_width));
        }

        if(isset($this->_height)) {
            if (ctype_digit($this->_height))
                array_push($attributes, attr("height", $this->_height . "%"));
            else
                array_push($attributes, attr("height", $this->_height));
        }
        
        array_push($attributes, attr("alt", $this->_alttext));
        array_push($attributes, attr("href", $this->_image));

        $image = html("img", batch($attributes));

        // Figure env
        if (isset($this->_caption)) {
            return html("figure", $image, html("figcaption", $this->_label . ": " . $this->_caption));
        }
        else {
            return $image;
        }
    }
}
