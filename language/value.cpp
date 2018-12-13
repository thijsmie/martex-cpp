#include "value.hpp"

Value::Value(ValueType type, std::string content) : type(type), content(content) {}

//multi
Value::Value(std::vector<Value> multicontent) : type(t_multi), multicontent(multicontent)
{
    content = "";
    for (Value v : multicontent)
        content += v.GetContent();
}
Value::Value(std::initializer_list<Value> multicontent) : Value(std::vector<Value>(multicontent)){};

//null
Value::Value() : type(t_null), content("") {}

//html
Value::Value(std::string tag, std::vector<Value> multicontent) : type(t_html), tag(tag), multicontent(multicontent) {}
Value::Value(std::string tag, std::initializer_list<Value> content) : Value(tag, std::vector<Value>(content)){};

//attr
Value::Value(std::string name, std::string value) : type(t_attr), tag(name), content(value) {}

std::string Value::GetContent() const
{
    std::string r_content = "";
    switch (type)
    {
    case t_string:
    case t_ampersand:
    case t_break:
        return content;
    case t_multi:
        for (Value v : multicontent)
            r_content += v.GetContent();
        return r_content;
    case t_html:
    {
        r_content = "<" + tag;

        bool hascontent = false;
        for (Value v : multicontent)
            if (v.GetType() == t_attr)
                r_content += " " + v.GetContent();
            else
                hascontent = true;

        if (hascontent)
        {
            r_content += ">";
            for (Value v : multicontent)
                if (v.GetType() != t_attr)
                    r_content += v.GetContent();
            r_content += "</" + tag + ">";
        }
        else
            r_content += "/>";
        return r_content;
    }
    case t_attr:
        r_content = tag + "=\"" + content + "\"";
        return r_content;
    default:
        return "";
    }
    return r_content;
}

std::string Value::GetTag() const { return tag; }

ValueType Value::GetType() const { return type; }

std::vector<Value> Value::GetValues() const { return multicontent; }

Value Value::Flattened()
{
    if (type != t_multi)
        return Value(std::vector<Value>({*this}));

    std::vector<Value> ret;

    for (Value v : multicontent)
    {
        std::vector<Value> flat = v.Flattened().GetValues();
        ret.insert(ret.end(), flat.begin(), flat.end());
    }

    return Value(ret);
}
