#include "language/environment.hpp"
#include "util/string_util.hpp"
#include "language/runtime_error.hpp"

class ItemizeEnvironment : public Environment
{
  protected:
    bool marker_set;
    std::string mytag;

  public:
    ItemizeEnvironment(std::shared_ptr<Environment> parent) : mytag("ul"), marker_set(false) 
    {
        enclosing = parent;
        is_root = false;
    }

    bool HasCommand(std::string name)
    {
        return (name == "item" or name == "setmarker");
    }

    Value RunCommandHere(std::shared_ptr<Environment> env, Token name, std::vector<Value> &arguments)
    {
        if (env.get() != (Environment *)this)
            throw RuntimeError(name, "cannot call in nested environment");

        if (name.GetLexeme() == "item")
        {
            if (arguments.size() > 1)
                throw RuntimeError(name, "takes zero/one argument");
            if (arguments.size() == 1)
                return Value("li", {arguments[0]});
            return Value(t_ampersand, "");
        }
        else
        {
            if (arguments.size() != 1)
                throw RuntimeError(name, "takes one argument");
            if (!arguments[0].IsPlain())
                throw RuntimeError(name, "takes plain argument");
            if (marker_set)
                throw RuntimeError(name, "only one setmarker per env");

            std::string type = arguments[0].GetContent();
            util::trim(type); // trim ws

            // uls
            if (type == "square")
            {
                marker_set = true;
                mytag = "ul";
                return Value("list-style-type", "square");
            }
            if (type == "bullet")
            {
                marker_set = true;
                mytag = "ul";
                return Value("list-style-type", "bullet");
            }
            if (type == "circle")
            {
                marker_set = true;
                mytag = "ul";
                return Value("list-style-type", "circle");
            }
            if (type == "none")
            {
                marker_set = true;
                mytag = "ul";
                return Value("list-style-type", "none");
            }

            // ols
            if (type == "numbers")
            {
                marker_set = true;
                mytag = "ol";
                return Value("type", "1");
            }
            if (type == "letters")
            {
                marker_set = true;
                mytag = "ol";
                return Value("type", "a");
            }
            if (type == "LETTERS")
            {
                marker_set = true;
                mytag = "ol";
                return Value("type", "A");
            }
            if (type == "roman")
            {
                marker_set = true;
                mytag = "ol";
                return Value("type", "i");
            }
            if (type == "ROMAN")
            {
                marker_set = true;
                mytag = "ol";
                return Value("type", "I");
            }
            throw RuntimeError(name, "Unknown marker type " + type);
        }
    }

    void StartEnvironment(Token, Value&){};
    Value EndEnvironment(Token end, Value &content)
    {
        std::vector<Value> cnt = content.Flattened().GetValues();

        std::vector<Value> items;
        Value* attr = nullptr;
        items.reserve(cnt.size());

        std::vector<Value> current_item;
        bool in_item = false;

        for (Value &v : cnt)
        {
            switch (v.GetType())
            {
            case t_null:
            case t_break:
                // don't care
                break;
            case t_string:
                if (!in_item && !util::wsonly(v.GetContent()))
                    throw RuntimeError(end, "Item environment contains loose data");
                if (in_item)
                    current_item.push_back(v);
                break;
            case t_attr:
                attr = &v;
                break;
            case t_html:
                if (v.GetTag() == "li")
                {
                    if (in_item)
                    {
                        items.push_back(Value("li", current_item));
                        current_item.clear();
                        in_item = false;
                    }
                    items.push_back(v);
                }
                else if (in_item)
                    current_item.push_back(v);
                else
                    throw RuntimeError(end, "Item environment contains loose data");
                break;
            case t_ampersand:
                if (in_item)
                {
                    items.push_back(Value("li", current_item));
                    current_item.clear();
                }
                in_item = true;
                break;
            case t_multi:
                throw RuntimeError(end, "Invalid type in itemize");
                break;
            }
        }

        if (in_item && current_item.size() > 0)
            items.push_back(Value("li", current_item));

        //Compress to string
        if (attr == nullptr)
            return Value(mytag, {Value(t_string, Value(std::move(items)).GetContent())});
        else
            return Value(mytag, {Value(t_string, Value(std::move(items)).GetContent()), *attr});
    }
};

class EnumerateEnvironment : public ItemizeEnvironment
{
  public:
    EnumerateEnvironment(std::shared_ptr<Environment> parent) : ItemizeEnvironment(parent) 
    {
        mytag = "ol";
    }
};