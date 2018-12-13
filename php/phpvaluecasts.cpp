#include "phpvaluecasts.hpp"
#include "language/runtime_error.hpp"
#include "language/token.hpp"

Php::Value CppToPhpSingle(Value v)
{
    // v garuanteed no t_multi

    Php::Value k;
    if (v.GetType() == t_string)
        k = v.GetContent();
    else if (v.GetType() == t_html)
    {
        k[0] = (int)v.GetType();
        k[1] = CppToPhp(v.GetValues());
    }
    else
    {
        k[0] = (int)v.GetType();
        k[1] = v.GetContent();
    }
    return k;
}

Value PhpToCpp(Php::Value value)
{
    if (value.isString()) 
    {
        std::string content = value;
        return Value(t_string, content);
    }

    if (value.isNull())
        return Value();
    
    Php::Value first = value[0];
    if(!first.isNumeric()) {
        std::vector<Value> a;
        for(auto v : value)
            a.push_back(PhpToCpp(v.second));
        return Value(a);
    }

    switch((int)first)
    {
        case t_null:
            return Value();
        case t_string:
        {
            std::string content = value[1];
            return Value(t_string, content);
        }
        case t_attr:
        {
            std::string name = value[1];
            std::string content = value[2];
            return Value(name, content);
        }
        case t_html:
        {
            std::string tag = value[1];
            std::vector<Value> a;

            for(int i = 2; i < value.count(); i++)
                a.push_back(PhpToCpp(value[i]));
            return Value(tag, a);
        }
        case t_multi:
        {
            std::vector<Value> a;
            Php::Value content = value[1];
            for(int i = 0; i < content.count(); i++)
                a.push_back(PhpToCpp(content[i]));
            return Value(a);
        }
        case t_break:
            return Value(t_break, "<br>");
        case t_ampersand:
            return Value(t_ampersand, "&amp;");
    }
    return Value();
}

Php::Value CppToPhp(Value v)
{
    Php::Array k;
    int i = 0;

    for (Value h : v.Flattened().GetValues())
    {
        k[i++] = CppToPhpSingle(h);
    }

    return k;
}

Php::Value CppToPhp(std::vector<Value> values)
{
    Php::Array k;
    int i = 0;

    for (Value h : values)
    {
        k[i++] = CppToPhp(h);
    }

    return k;
}

Php::Value html(Php::Parameters &params)
{
    std::string tag = params[0];
    Php::Array content = std::vector<Php::Value>(params.begin() + 1, params.end());

    Php::Array ret;
    ret[0] = t_html;
    ret[1] = tag;
    ret[2] = content;

    return ret;
}

Php::Value value(Php::Parameters &params)
{
    return std::vector<Php::Value>(params.begin(), params.end());
}

Php::Value attr(Php::Parameters &params)
{
    return std::vector<Php::Value>({t_attr, params[0], params[1]});
}

Php::Value batch(Php::Parameters &params)
{
    if (params.size() == 1 && params[0].isArray())
    {
        Php::Array a = params[0];
        Php::Array ret;
        ret[0] = t_multi;
        ret[1] = a;
        return ret;
    }
    else
    {
        Php::Array a = std::vector<Php::Value>(params.begin(), params.end());
        Php::Array ret;
        ret[0] = t_multi;
        ret[1] = a;
        return ret;
    }
}

Php::Value ampersand(Php::Parameters &params)
{
    return Php::Array({t_ampersand, "&amp;"});
}

Php::Value newline(Php::Parameters &params)
{
    return Php::Array({t_ampersand, "<br/>"});
}