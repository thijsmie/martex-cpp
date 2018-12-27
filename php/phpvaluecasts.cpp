#include "phpvaluecasts.hpp"
#include "language/runtime_error.hpp"
#include "language/token.hpp"
#include <chrono>

Php::Value CppToPhpSingle(const Value &v)
{
    // v garuanteed no t_multi

    Php::Value k;
    if (v.GetType() == t_html)
    {
        k[0] = t_html;
        k[1] = v.GetTag();
        k[2] = CppToPhp(std::move(v.GetValues()));
    }
    else if (v.GetType() == t_attr)
    {
        k[0] = t_attr;
        k[1] = v.GetTag();
        k[2] = v.GetRawContent();
    }
    else
    {
        k[0] = (int)v.GetType();
        k[1] = v.GetContent();
    }
    return k;
}


Value PhpToCppi(const Php::Value &value)
{
    if (value.isString())
    {
        std::string content = value;
        return Value(t_string, content);
    }

    if (value.isNull())
        return Value();
    
    Php::Value first = value[0];
    if (!first.isNumeric())
    {
        std::vector<Value> a;
        for (auto v : value)
            a.push_back(PhpToCppi(v.second));
        return Value(std::move(a));
    }

    switch ((int)first)
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
        Value a = PhpToCpp(value[2]);

        return Value(tag, a.GetValues());
    }
    case t_multi:
    {
        std::vector<Value> a;
        Php::Value content = value[1];
        a.reserve(content.count());
        for (int i = 0; i < content.count(); i++)
            a.push_back(PhpToCppi(content[i]));
        return Value(std::move(a));
    }
    case t_break:
        return Value(t_break, "<br>");
    case t_ampersand:
        return Value(t_ampersand, "&amp;");
    }
    return Value();
}

Value PhpToCpp(const Php::Value &value)
{
    std::chrono::high_resolution_clock::time_point a = std::chrono::high_resolution_clock::now();
    Value v = PhpToCppi(value);
    std::chrono::high_resolution_clock::time_point b = std::chrono::high_resolution_clock::now();
    int d = (std::chrono::duration_cast<std::chrono::microseconds>(b-a)).count();
    /*
    if (value.count() > 2)
        Php::out << value[0] << ' ' << d << std::endl;
    else 
        Php::out << "Array " << d << std::endl;*/
    return v;
}

Php::Value CppToPhp(const Value &v)
{
    Php::Value k;
    int i = 0;

    for (Value h : v.Flattened().GetValues())
    {
        k[i++] = CppToPhpSingle(h);
    }

    return k;
}

Php::Value CppToPhp(const std::vector<Value> &values)
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
    //tag
    const char* v1 = params[0];
    const int s1 = params[0].size();
    //content
    const char* v2 = params[1];
    const int s2 = params[1].size();

    Php::Value v;
    char* b = v.reserve(s1 + s2 + 3);

    b[0] = t_html;
    b[1] = s1;
    memcpy(b+1, v1, s1);
    b[s1+1] = s2;
    memcpy(b+s1+2, v2, s2);
    return v;
}

Php::Value string(Php::Parameters &params)
{
    const char* v1 = params[0];
    const int s1 = params[0].size();

    Php::Value v; 
    char* b = v.reserve(2+s1);
    b[0] = t_ampersand;
    b[1] = s1;
    memcpy(b+2, v1, s1);
    return v;
}

Php::Value attr(Php::Parameters &params)
{
    const char* v1 = params[0];
    const int s1 = params[0].size();
    const char* v2 = params[1];
    const int s2 = params[1].size();

    Php::Value v;
    char* b = v.reserve(s1 + s2 + 3);

    b[0] = t_attr;
    b[1] = s1;
    memcpy(b+1, v1, s1);
    b[s1+1] = s2;
    memcpy(b+s1+2, v2, s2);
    return v;
}

Php::Value ampersand(Php::Parameters &params)
{
    Php::Value v; 
    char* b = v.reserve(1);
    b[0] = t_ampersand;
    return v;
}

Php::Value newline(Php::Parameters &params)
{
    Php::Value v; 
    char* b = v.reserve(1);
    b[0] = t_break;
    return v;
}