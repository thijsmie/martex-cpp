<?php
namespace MarTeXcpp;

require_once FOLDER_MODELS . 'document/document.php';
require_once FOLDER_MODELS . 'user/user.php';
require_once FOLDER_MODELS . 'photo/photoAlbum.php';


class MarieModule extends Module {
    private $_usercache = array();
    private $_docscache = array();
    
    private function logged_in()
    {
        // You could have two separate setups with a logged in and a not logged in version
        // We will use this module in email first and then we will use it in a context where we
        // Live render whatever we need so I won't bother implementing that
        return app()->isLoggedIn();
    }

    // Indicate to martex which functions we implement
    public function globals() {
        return array("user", "person", "publicperson", "document", "photo", "album");
    }
    
    // User, takes one PlainText argument
    public $argsfor_user = array(PlainText);
    public function user($env, $args)
    {
        $username = $args[0];
        
        if (array_key_exists($username, $this->usercache))
            return $this->usercache[$username];
        
        $user = null;
        if (array_key_exists($username, $this->usercache))
        {
            $user = $this->usercache[$username];
        }
        else
        {
            $user = \models\user::getByUsername($username);
            $this->usercache[$username] = $user;
        }

        if (!($this->logged_in())) {
            // In future respect said users privacy settings instead.
            return html("span", 
                attr("class", "martexuser"), 
                mstr("[verborgen]")
            );
        }

        if ($user == null) {
            return html("span", 
                attr("class", "martexuser"), 
                mstr("[bestaat niet]")
            );
        }

        return html("span", 
            attr("class", "martexuser"), 
            html("a",
                attr("href", "/kopstukken/user/" . $user->getID()),
                mstr($user->getName())
            )
        );
    }
    
    // Person, takes one PlainText argument
    public $argsfor_person = array(PlainText);
    public function person($env, $args)
    {
        return $this->user($env, $args);
    }
    
    // Publicperson, takes one PlainText argument
    public $argsfor_publicperson = array(PlainText);
    public function publicperson($env, $args)
    {
        $username = $args[0];
        
        $user = null;
        if (array_key_exists($username, $this->usercache))
        {
            $user = $this->usercache[$username];
        }
        else
        {
            $user = \models\user::getByUsername($username);
            $this->usercache[$username] = $user;
        }

        if ($user == null) {
            return html("span", 
                attr("class", "martexuser"), 
                mstr("[bestaat niet]")
            );
        }
        
        if (!($this->logged_in())) {
            // In future respect said users privacy settings instead.
            return html("span", 
                attr("class", "martexuser"), 
                mstr($user->getName())
            );
        }

        return html("span", 
            attr("class", "martexuser"), 
            html("a",
                attr("href", "/kopstukken/user/" . $user->getID()),
                mstr($user->getName())
            )
        );
    }
    
    // Document, takes one PlainText argument
    public $argsfor_document = array(PlainText);
    public function document($env, $args)
    {
        $docname = $args[0];
        
        $doc = null;
        if (array_key_exists($docname, $this->docscache))
        {
            $doc = $this->docscache[$docname];
        }
        else
        {
            $doc = \models\document::getByShortname($docname);
            $this->docscache[$docname] = $doc;
        }

        if ($doc == null) {
            return html("span", 
                attr("class", "martexdocument"), 
                mstr("[bestaat niet]")
            );
        }
        
        if ($doc->getPrivate() && !($this->logged_in())) {
            return html("span", 
                attr("class", "martexdocument"), 
                mstr("[verborgen]")
            );
        }

        return html("span", 
            attr("class", "martexdocument"), 
            html("a",
                attr("href", "/document/download/" . $doc->getID()),
                mstr($doc->getName())
            )
        );
    }
    
    
    // email, takes one PlainText argument
    public $argsfor_email = array(PlainText);
    public function email($env, $args)
    {
        if ($this->logged_in())
            return html("a",
                attr("href", "mailto:" + $args[0]),
                mstr($args[0])
            );
        return mstr("[verborgen]");
    }
    
    public $argsfor_album = array(PlainText);
    public function album($env, $args)
    {
        $album = \models\photoAlbum::byReference($args[0]);

        if ($album == null)
            return mstr("[bestaat niet]");

        if ($album->getPrivate() && !$this->logged_in())
            return mstr("[verborgen]");

        return html("div",
            attr("class", "col-md-4 float-left align-items-stretch"),
            html("div",
                attr("class", "card mb-3"),
                html("a",
                    attr("href", "/photo/album/" . $album->getID()),
                    html("img",
                        attr("src", \models\photoAlbum::photo_thumb($album->getCoverphoto())),
                        attr("class", "card-img")
                    ),
                    html("div",
                        attr("class", "card-img-overlay"),
                        html("h5",
                            attr("class", "card-title text-white text-shade"),
                            mstr($album->getName())
                        )
                    )
                )
            )
        );
    }
    
    public $argsfor_photo = array(PlainText);
    public function photo($env, $args)
    {
        return mstr(\models\photoAlbum::photo_uri($args[0]));
    }
}

