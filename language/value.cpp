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
Value::Value(std::string tag, std::vector<Value> multicontent) : type(t_html), multicontent(multicontent)
{
    content = "<" + tag;

    bool hascontent = false;
    for (Value v : multicontent)
        if (v.GetType() == t_attr)
            content += " " + v.GetContent();
        else
            hascontent = true;

    if (hascontent)
    {
        content += ">";
        for (Value v : multicontent)
            if (v.GetType() != t_attr)
                content += v.GetContent();
        content += "</" + tag + ">";
    }
    else
        content += "/>";
}
Value::Value(std::string tag, std::initializer_list<Value> content) : Value(tag, std::vector<Value>(content)){};

//attr
Value::Value(std::string name, std::string value) : type(t_attr)
{
    content = name + "=\"" + value + "\"";
}

std::string Value::GetContent() const { return content; }

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
