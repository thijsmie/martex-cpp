<?php
namespace MarTeX;


class ItemizeModule extends Module {
    public function environments() {
        return array(
            "itemize" => "\\MarTeX\\ItemizeEnvironment", 
            "enumerate" => "\\MarTeX\\EnumerateEnvironment"
        );
    }
}
    
class ItemizeEnvironment extends Environment
{
    protected $marker_set = false;
    protected $html_tag;

    public function locals()
    {
        return array("setmarker", "item");
    }

    public function begin($bracket)
    {
        $this->html_tag = "ul";
    }

    public function setmarker($env, $args)
    {
        if ($env != $this)
            throw new \Exception("Cannot call setmarker in nested environment.");

        if ($this->marker_set)
            throw new \Exception("Only one setmarker per list.");

        if (count($args) != 1 && !is_string($args[0][0][1]))
            throw new \Exception("Setmarker takes one string as argument.");

        $this->marker_set = true;

        switch($args[0][0][1]) {
            case "square":
                $this->html_tag = "ul";
                return attr("list-style-type", "square");
            case "bullet":
                $this->html_tag = "ul";
                return attr("list-style-type", "square");
            case "circle":
                $this->html_tag = "ul";
                return attr("list-style-type", "square");
            case "none":
                $this->html_tag = "ul";
                return attr("list-style-type", "none");
            case "numbers":
                $this->html_tag = "ol";
                return attr("type", "1");
            case "letters":
                $this->html_tag = "ol";
                return attr("type", "a");
            case "LETTERS":
                $this->html_tag = "ol";
                return attr("type", "A");
            case "roman":
                $this->html_tag = "ol";
                return attr("type", "i");
            case "ROMAN":
                $this->html_tag = "ol";
                return attr("type", "I");
            default:
                throw new \Exception("Unknown marker ". $args[0][0][1]);
        }
    }

    public function item($env, $args)
    {
        if ($env != $this)
            throw new \Exception("Cannot call item in nested environment.");

        if (is_null($args))
            return ampersand();

        if (count($args) != 1)
            throw new \Exception("item takes one argument");

        return html("li", $args[0]);
    }

    public function end($content)
    {
        $in_item = false;
        $items = array();
        $item = array();

        foreach($content as $p_item)
        {
            switch($p_item[0])
            {
                case TypeNull:
                case TypeBreak:
                //ignore
                break;
                case TypeString:
                    if (!$in_item && !ctype_space($p_item[1]))
                        throw new \Exception("Item environment contains loose data.");
                    if ($in_item)
                        array_push($item, $p_item);
                break;
                case TypeAttr:
                    array_push($items, $p_item);
                break;
                case TypeHtml:
                    if ($p_item[1] == "li")
                    {
                        if ($in_item)
                        {
                            array_push($items, html("li", batch($item)));
                            $item = array();
                            $in_item = false;
                        }
                        array_push($items, $p_item);
                    }
                    else if ($in_item)
                        array_push($item, $p_item);
                    else
                        throw new \Exception("Item environment contains non-list items");
                break;
                case TypeAmpersand:
                    if ($in_item)
                    {
                        array_push($items, html("li", batch($item)));
                        $item = array();
                    }
                    else {
                        $in_item = true;
                    }
                break;
            }
        }
        
        if ($in_item && count($item) > 0)
            array_push($items, html("li", batch($item)));

        return html($this->html_tag, batch($items));
    }
}

class EnumerateEnvironment extends ItemizeEnvironment
{
    public function begin($bracket)
    {
        $this->html_tag = "ol";
    }
}