#pragma once

#include "core/util/cppenvironment.hpp"
#include "core/util/value_util.hpp"
#include "core/util/string_util.hpp"
#include "core/language/runtime_error.hpp"

class ItemizeEnvironment;
typedef util::CppEnvironment<ItemizeEnvironment> ParentItemize;
class ItemizeEnvironment : public ParentItemize
{
  protected:
    bool marker_set;
    std::string mytag;

  public:
    ItemizeEnvironment(std::shared_ptr<Environment> parent) : ParentItemize(parent), marker_set(false), mytag("ul")
    {
        AddMethod("item", &ItemizeEnvironment::item);
        AddMethod("setmarker", &ItemizeEnvironment::setmarker);
    }

    Value item(Token name, std::vector<Value> arguments)
    {
        if (arguments.size() > 1)
            throw RuntimeError(name, "takes zero/one argument");
        if (arguments.size() == 1)
            return Value("li", std::move(arguments));
        return Value(t_ampersand, "");
    }

    Value setmarker(Token name, std::vector<Value> arguments)
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
            return Value("style", "list-style-type: square;");
        }
        if (type == "bullet")
        {
            marker_set = true;
            mytag = "ul";
            return Value("style", "list-style-type: bullet");
        }
        if (type == "circle")
        {
            marker_set = true;
            mytag = "ul";
            return Value("style", "list-style-type: circle");
        }
        if (type == "none")
        {
            marker_set = true;
            mytag = "ul";
            return Value("style", "list-style-type: none");
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

    void StartEnvironment(Token, Value){};
    Value EndEnvironment(Token end, Value content)
    {
        std::vector<Value> items;
        Value *attr = nullptr;
        items.reserve(content.multicontent.size());

        std::vector<Value> current_item;
        bool in_item = false;

        for (Value &v : content.multicontent)
        {
            switch (v.GetType())
            {
            case t_null:
            case t_break:
            case t_info:
                // don't care
                break;
            case t_string:
                if (!in_item && !util::wsonly(v.GetContent()))
                    throw RuntimeError(end, "Item environment contains loose data");
                if (in_item)
                    current_item.push_back(std::move(v));
                break;
            case t_attr:
                attr = &v;
                break;
            case t_html:
                if (v.GetTag() == "li")
                {
                    if (in_item)
                    {
                        util::trimws(current_item);
                        items.emplace_back("li", std::move(current_item));
                        current_item.clear();
                        in_item = false;
                    }
                    items.push_back(std::move(v));
                }
                else if (in_item)
                    current_item.push_back(std::move(v));
                else
                    throw RuntimeError(end, "Item environment contains loose data");
                break;
            case t_ampersand:
                if (in_item)
                {
                    util::trimws(current_item);
                    items.emplace_back("li", std::move(current_item));
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
        {
            util::trimws(current_item);
            items.emplace_back("li", std::move(current_item));
        }

        if (attr == nullptr)
        {
            return Value(mytag, std::move(items));
        }
        else
        {
            items.emplace_back(attr->GetTag(), attr->GetRawContent());
            return Value(mytag, std::move(items));
        }
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