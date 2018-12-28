<?php
namespace MarTeX;


class DocumentModule extends Module {
    private $counters = array();

    public function globals() {
        return array(
            "href", "col", "color", "colour"
        );
    }

    public function environments() {
        return array("paragraph" => "\\MarTeX\\ParagraphEnvironment");
    }

    public function href($env, $args)
    {
        return html("a", attr("href", $args[0][0][1]), mstr($args[1][0][1]));
    }

    public function col($env, $args)
    {
        return $this->colour($env, $args);
    }

    public function color($env, $args)
    {
        return $this->colour($env, $args);
    }

    public static function cfilter($value) 
    {
        return preg_replace("/[^a-zA-Z0-9]+/", "", $value);
    }

    public function colour($env, $args)
    {
        $coltype = "";
        $coldef = "";
        $txt = array();

        if (count($args) < 2)
            throw new \Exception("Invalid colour");

        if (count($args) == 2) {
            $coldef = $args[0][0][1];
            $txt = $args[1][0][1];

            if ($coldef[0] === "#") {
                $coltype = "hex";
            }
            else if (substr_count($coldef, ',') === 2) {
                $coltype = "rgb";
            }
            else if (substr_count($coldef, ',') === 3) {
                $coltype = "rgba";
            }
        }
        else if (count($args) === 3) {
            $coltype = $args[0][0][1];
            $coldef = $args[1][0][1];
            $txt = $args[2][0][1];
        }

        $color = "";
        switch($coltype) {
            case "hex":
                $color = "#" . self::cfilter($coldef);
                break;
            case "rgb":
                $bins = explode(',' , $coldef);
                if (count($bins) !== 3) {
                    throw new \Exception("Invalid rgb colour definition.");
                }
                $color =  "rgb(" . 
                    self::cfilter($bins[0]) . "," .
                    self::cfilter($bins[1]) . "," .
                    self::cfilter($bins[2]) . ")";
                break;
            case "rgba":
                $bins = explode(',', $coldef);
                if (count($bins) !== 4) {
                    throw new \Exception("Invalid rgba colour definition.");
                }
                $color =  "rgba(" . 
                    self::cfilter($bins[0]) . "," .
                    self::cfilter($bins[1]) . "," .
                    self::cfilter($bins[2]) . "," .
                    self::cfilter($bins[3]) . ")";
                break;
            case "hsl":
                $bins = explode(',', $coldef);
                if (count($bins) !== 3) {
                    throw new \Exception("Invalid hsl colour definition.");
                }
                $color = "hsl(" . 
                    self::cfilter($bins[0]) . "," .
                    self::cfilter($bins[1]) . "%," .
                    self::cfilter($bins[2]) . "%)";
                break;
            case "hsla":
                $bins = explode(',', $coldef);
                if (count($bins) !== 4) {
                    throw new \Exception("Invalid hsla colour definition.");
                }
                $color = "hsla(" . 
                    self::cfilter($bins[0]) . "," .
                    self::cfilter($bins[1]) . "%," .
                    self::cfilter($bins[2]) . "%," .
                    self::cfilter($bins[3]) . ")";
                break;
            default:
                throw new \Exception("Unknown colouring");
        }

        return html("span", attr("style", "color:".$color) . mstr($txt));
    }    
}

class ParagraphEnvironment extends Environment
{
    public function end($content)
    {
        return array(TypeHtml, "p", $content);
    }
}