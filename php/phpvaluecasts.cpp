#include "phpvaluecasts.hpp"

Php::Value CppToPhpSingle(Value v)
{
    // v garuanteed no t_multi
    Php::Value k;
    k[0] = (int)v.GetType();
    k[1] = v.GetContent();
    return k;
}

Value PhpToCpp(Php::Value values)
{
    std::vector<Value> ret;
    for (auto &v : values)
    {
        const char *buffer = v.second[1];
        size_t size = v.second[1].size();
        std::string content(buffer, size);
        ret.push_back(Value((ValueType)((int)v.second[0]), content));
    }

    return ret;
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