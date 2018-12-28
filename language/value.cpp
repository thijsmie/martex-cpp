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

        std::vector<Value> toloop = Value(multicontent).Flattened().GetValues();

        bool hascontent = false;
        for (Value v : toloop)
            if (v.GetType() == t_attr)
                r_content += " " + v.GetContent();
            else if (v.GetType() != t_null)
                hascontent = true;

        if (hascontent)
        {
            r_content += ">";
            for (Value v : toloop)
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

bool Value::IsPlain() const
{
    switch (type)
    {
        case t_string:
        case t_break:
        case t_null:
            return true;
        case t_attr:
        case t_ampersand:
        case t_html:
            return false;
        case t_multi:
        {
            for (Value v : multicontent)
                if (!v.IsPlain())
                    return false;
            return true;
        }
    }
    return true;
}

std::string Value::GetTag() const { return tag; }
std::string Value::GetRawContent() const { return content; }
ValueType Value::GetType() const { return type; }

std::vector<Value> Value::GetValues() const { return multicontent; }

Value Value::Flattened() const
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

std::string valueTypes[] = {
    "t_null",
    "t_string",
    "t_ampersand",
    "t_break",
    "t_multi",
    "t_html",
    "t_attr"
};

std::string ValueTypeName(ValueType v)
{
    return valueTypes[v];
}