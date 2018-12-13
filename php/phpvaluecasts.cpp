#include "phpvaluecasts.hpp"
#include "language/runtime_error.hpp"
#include "language/token.hpp"


Php::Value CppToPhpSingle(Value v)
{
    // v garuanteed no t_multi
    
    Php::Value k;
    if (v.GetType() == t_html)
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

Value PhpToCpp(Php::Value values)
{

    if (!values.isArray())
    {
        // It's only a string
        std::string tmp = values;
        return Value(t_string, tmp);
    }

    Php::Value first = values[0];
    if (first.isNumeric())
    {
        Php::Array a;
        a[0] = values;
        return PhpToCpp(a);
    }

    try 
    {
        std::vector<Value> ret;

        for (auto &v : values)
        {
            if ((!v.second.isArray()) && v.second.isString())
            {
                std::string content = v.second;
                ret.push_back(Value(t_string, content));
            }
            else if (v.second[0].isNumeric() && v.second[1].isString())
            {
                if (v.second.size() == 3 && (int)v.second[0] == t_html)
                {
                    //html
                    std::string tag = v.second[1];
                    Value content = PhpToCpp(v.second[2]);
                    ret.push_back(Value(tag, content.GetValues()));
                }
                else {
                    std::string content = v.second[1];
                    ret.push_back(Value((ValueType)((int)v.second[0]), content));
                }
            }
            else if (v.second[0].isString() && v.second[1].isString())
            {
                std::string one = v.second[0];
                std::string two = v.second[1];
                ret.push_back(Value(one, two));
            }
            
        }

        return ret;
    }
    catch(...)
    {
        throw RuntimeError(Token(COMMAND, "output", -1), "Invalid data returned from command.");
    }

    return Value();
}

Php::Value CppToPhp(Value v)
{
    Php::Value k;
    int i = 0;

    for (Value h : v.Flattened().GetValues())
    {
        k[i++] = CppToPhpSingle(h);
    }

    return k;
}

Php::Value CppToPhp(std::vector<Value> values)
{
    Php::Value k;
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
    Php::Array content = std::vector<Php::Value>(params.begin()+1, params.end());

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

Php::Value batch(Php::Parameters &params)
{
    return std::vector<Php::Value>(params.begin(), params.end());
}

Php::Value ampersand(Php::Parameters &params)
{
    return Php::Array({t_ampersand, "&amp;"});
}

Php::Value newline(Php::Parameters &params)
{
    return Php::Array({t_ampersand, "<br/>"});
}