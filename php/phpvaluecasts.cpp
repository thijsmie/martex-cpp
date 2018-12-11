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
    try 
    {
        std::vector<Value> ret;
        for (auto &v : values)
        {
            if (v.second[0].isNumeric() && v.second[1].isString())
            {
                const char *buffer = v.second[1];
                size_t size = v.second[1].size();
                std::string content(buffer, size);
                ret.push_back(Value((ValueType)((int)v.second[0]), content));
            }
            else if (v.second[0].isString() && v.second[1].isString())
            {
                const char *buffer1 = v.second[0];
                size_t size1 = v.second[0].size();
                std::string content1(buffer1, size1);
                const char *buffer2 = v.second[1];
                size_t size2 = v.second[1].size();
                std::string content2(buffer2, size2);
                ret.push_back(Value(content1, content2));
            }
            else
            {
                const char *buffer = v.second[0];
                size_t size = v.second[0].size();
                std::string tag(buffer, size);
                ret.push_back(Value(tag, PhpToCpp(v.second[1]).GetValues()));
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