#include "phpvaluecasts.hpp"
#include "language/runtime_error.hpp"
#include "language/token.hpp"
#include "encoding/buffer.hpp"
#include <chrono>
#include <cstdint>

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

Value PhpToCppSlow(Php::Value value)
{
    if (value.isNull())
        return Value();

    Php::Value first = value[0];
    if (!first.isNumeric())
    {
        std::vector<Value> a;
        for (auto v : value)
            a.push_back(PhpToCpp(v.second));
        return Value(a);
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
        std::vector<Value> a;

        for (int i = 2; i < value.count(); i++)
            a.push_back(PhpToCpp(value[i]));
        return Value(tag, a);
    }
    case t_multi:
    {
        std::vector<Value> a;
        Php::Value content = value[1];
        for (int i = 0; i < content.count(); i++)
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

std::string PhpAsBytesI(Php::Value value)
{
    if (value.isNull())
        return std::string((char)t_null);

    Php::Value first = value[0];
    if (!first.isNumeric())
    {
        std::string a;
        for (auto v : value)
            a += PhpAsBytesI(v.second);
        return a;
    }

    const char type = (int)first;
    switch ((int)type)
    {
    case t_null:
    case t_ampersand:
    case t_break:
        return std::string(1, type);
    case t_string:
    {
        std::string content = value[1];

        unsigned char b[5];
        int pos = 0;
        b[pos++] = t_string;
        serialize_uint32_t(b, pos, content.size());

        return std::string((char*)b, 5) + content;
    }
    case t_attr:
    {
        std::string name = value[1];
        std::string content = value[2];

        unsigned char b[9];
        int pos = 0;
        b[pos++] = t_attr;
        serialize_uint32_t(b, pos, name.size());
        serialize_uint32_t(b, pos, content.size());

        return std::string((char*)b, 5) + name + std::string((char*)b+5, 4) + content;
    }
    case t_html:
    {
        std::string tag = value[1];
        std::string content = PhpAsBytesI(value[2]);

        unsigned char b[9];
        int pos = 0;
        b[pos++] = t_html;
        serialize_uint32_t(b, pos, tag.size());
        serialize_uint32_t(b, pos, content.size());

        return std::string((char*)b, 5) + tag + std::string((char*)b+5, 4) + content;
    }
    case t_multi:
    {
        std::string a;
        Php::Value content = value[1];
        for (int i = 0; i < content.count(); i++)
            a += PhpAsBytesI(content[i]);
        return a;
    }
    }
}

Php::Value PhpAsBytes(Php::Parameters &params)
{
    Php::Value value = params[0];
    return PhpAsBytesI(value);
}

Value PhpToCppSingle(const unsigned char *b, int &pos, const uint32_t end)
{
    const char type = b[pos++];

    switch (type)
    {
    case t_ampersand:
        return Value(t_ampersand, "&amp;");
    case t_break:
        return Value(t_break, "<br/>");
    case t_null:
        return Value();
    case t_string:
    {
        std::string res = deserialize_sized_string(b, pos);
        return Value(t_string, std::move(res));
    }
    case t_attr:
    {
        std::string res1 = deserialize_sized_string(b, pos);
        std::string res2 = deserialize_sized_string(b, pos);

        return Value(std::move(res1), std::move(res2));
    }
    case t_html:
    {
        std::string res1 = deserialize_sized_string(b, pos);
 
        const uint32_t size2 = deserialize_uint32_t(b, pos);
        std::vector<Value> content = PhpToCppMulti(b, pos, size2 + pos);

        return Value(std::move(res1), std::move(content));
    }
    default:
        throw Php::Exception("Invalid return value");
    }
}

std::vector<Value> PhpToCppMulti(const unsigned char *b, int &pos, const uint32_t end)
{
    std::vector<Value> a;

    while (pos < end)
    {
        //Php::out << ValueTypeName((ValueType)b[pos]);
        a.push_back(PhpToCppSingle(b, pos, end));
        //Php::out << std::endl;
    }

    return a;
}

Value PhpToCppi(const Php::Value &value)
{
    if (value.isArray())
        return PhpToCppSlow(value);

    const char *v1 = value;
    const uint32_t s1 = value.size();

    int pos = 0;
    std::vector<Value> res = PhpToCppMulti((unsigned char*)v1, pos, s1);

    if (res.size() == 0)
        return Value();
    else if (res.size() == 1)
        return res[0];
    else
        return Value(res);
}

Value PhpToCpp(const Php::Value &value)
{
    //std::chrono::high_resolution_clock::time_point a = std::chrono::high_resolution_clock::now();
    Value v = PhpToCppi(value);
    //std::chrono::high_resolution_clock::time_point b = std::chrono::high_resolution_clock::now();
    //int d = (std::chrono::duration_cast<std::chrono::microseconds>(b - a)).count();
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
    if (params.size() == 1)
    {
        unsigned char b[20000];

        //tag
        const char *v1 = params[0];
        const uint32_t s1 = params[0].size();

        int pos = 0;

        b[pos++] = t_html;
        serialize_uint32_t(b, pos, s1);
        memcpy((char*)b + pos, v1, s1);
        serialize_uint32_t(b, pos, 1);
        b[pos++] = t_null;

        return Php::Value((char*)b, pos);
    }

    //tag
    const char *v1 = params[0];
    const uint32_t s1 = params[0].size();
    //content
    const char *v2 = params[1];
    const uint32_t s2 = params[1].size();

    // alloc
    const uint32_t totalsize = s1 + s2 + 1 + 2 * sizeof(uint32_t);
    unsigned char* b = (unsigned char*) malloc(totalsize);

    int pos = 0;

    b[pos++] = t_html;
    serialize_sized_string(b, pos, v1, s1);
    serialize_sized_string(b, pos, v2, s2);

    Php::Value ret((char*)b, pos);

    // free
    free(b);

    return ret;
}

Php::Value mstr(Php::Parameters &params)
{
    const char *v1 = params[0];
    const uint32_t s1 = params[0].size();

    // alloc
    const uint32_t totalsize = s1 + 1 + sizeof(uint32_t);
    unsigned char* b = (unsigned char*) malloc(totalsize);

    int pos = 0;

    b[pos++] = t_string;
    serialize_sized_string(b, pos, v1, s1);

    Php::Value ret((char*)b, pos);

    // free
    free(b);

    return ret;
}

Php::Value attr(Php::Parameters &params)
{
    //attribute
    const char *v1 = params[0];
    const uint32_t s1 = params[0].size();
    //content
    const char *v2 = params[1];
    const uint32_t s2 = params[1].size();

    // alloc
    const uint32_t totalsize = s1 + s2 + 1 + 2 * sizeof(uint32_t);
    unsigned char* b = (unsigned char*) malloc(totalsize);

    int pos = 0;

    b[pos++] = t_attr;
    serialize_sized_string(b, pos, v1, s1);
    serialize_sized_string(b, pos, v2, s2);

    Php::Value ret((char*)b, pos);

    // free
    free(b);

    return ret;
}

Php::Value ampersand(Php::Parameters &params)
{
    return Php::Value((char)t_ampersand);
}

Php::Value newline(Php::Parameters &params)
{
    return Php::Value((char)t_break);
}