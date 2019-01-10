#include "phpvaluecasts.hpp"
#include "language/runtime_error.hpp"
#include "language/token.hpp"
#include "util/buffer.hpp"
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
        k[2] = CppToPhp(v.multicontent);
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
        std::vector<Value> a;

        for (int i = 2; i < value.count(); i++)
            a.push_back(PhpToCpp(value[i]));
        return Value(tag, std::move(a));
    }
    case t_multi:
    {
        std::vector<Value> a;
        Php::Value content = value[1];
        for (int i = 0; i < content.count(); i++)
            a.push_back(PhpToCpp(content[i]));
        return Value(std::move(a));
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
        return std::string(1, (char)(t_null+32));

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
        return std::string(1, type+32);
    case t_string:
    {
        std::string content = value[1];

        unsigned char b[5];
        uint32_t pos = 0;
        serialize_byte(b, pos, (uint8_t)t_string);
        serialize_uint32_t(b, pos, content.size());

        return std::string((char*)b, 5) + content;
    }
    case t_attr:
    {
        std::string name = value[1];
        std::string content = value[2];

        unsigned char b[9];
        uint32_t pos = 0;
        serialize_byte(b, pos, (uint8_t)t_attr);
        serialize_uint32_t(b, pos, name.size());
        serialize_uint32_t(b, pos, content.size());

        return std::string((char*)b, 5) + name + std::string((char*)b+5, 4) + content;
    }
    case t_html:
    {
        std::string tag = value[1];
        std::string content = PhpAsBytesI(value[2]);

        unsigned char b[9];
        uint32_t pos = 0;
        serialize_byte(b, pos, (uint8_t)t_html);
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

Value PhpToCppSingle(const unsigned char *b, uint32_t &pos, const uint32_t end)
{
    const uint8_t type = deserialize_byte(b, pos);

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

std::vector<Value> PhpToCppMulti(const unsigned char *b, uint32_t &pos, const uint32_t end)
{
    std::vector<Value> a;

    while (pos < end)
    {
        a.push_back(Value(b, pos));
    }

    return a;
}

Value PhpToCpp(const Php::Value &value)
{
    if (value.isArray())
        return PhpToCppSlow(value);

    if (!value.isString())
        return Value();

    const char *v1 = value;
    const uint32_t s1 = value.size();

    uint32_t pos = 0;
    std::vector<Value> res = PhpToCppMulti((unsigned char*)v1, pos, s1);

    if (res.size() == 0)
        return Value();
    else if (res.size() == 1)
        return std::move(res[0]);
    else
        return Value(std::move(res));
}

Php::Value CppToPhp(const Value &v)
{
    Php::Value k;
    int i = 0;

    if (v.GetType() == t_multi)
    {
        for (const Value &h : v.multicontent)
        {
            k[i++] = CppToPhpSingle(h);
        }
    }
    else
    {
        k = CppToPhpSingle(v);
    }

    return k;
}

Php::Value CppToPhp(const std::vector<Value> &values)
{
    Php::Value k;
    int i = 0;

    for (const Value &h : values)
    {
        k[i++] = CppToPhp(h);
    }

    return k;
}

Php::Value CppToPhpBytes(const Value &v)
{
    uint32_t size = v.ByteSize();

    uint32_t pos = 0;
    unsigned char *buffer = (unsigned char*)malloc(size);

    v.WriteOut(buffer, pos);

    Php::Value a((char*)buffer, size);

    free(buffer);

    return a;
}

Php::Value html(Php::Parameters &params)
{
    if (params.size() == 1)
    {
        //tag
        const char *v1 = params[0];
        const uint32_t s1 = params[0].size();

        // alloc
        const uint32_t totalsize = s1 + 1 + 1 + 2 * sizeof(uint32_t);
        unsigned char* b = (unsigned char*) malloc(totalsize);

        uint32_t pos = 0;

        serialize_byte(b, pos, (uint8_t)t_html);
        serialize_sized_string(b, pos, v1, s1);
        serialize_uint32_t(b, pos, 1);
        serialize_byte(b, pos, (uint8_t)t_null);

        Php::Value ret((char*)b, pos);

        // free
        free(b);
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

    uint32_t pos = 0;

    serialize_byte(b, pos, (uint8_t)t_html);
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

    uint32_t pos = 0;

    serialize_byte(b, pos, (uint8_t)t_string);
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

    uint32_t pos = 0;

    serialize_byte(b, pos, (uint8_t)t_attr);
    serialize_sized_string(b, pos, v1, s1);
    serialize_sized_string(b, pos, v2, s2);

    Php::Value ret((char*)b, pos);

    // free
    free(b);

    return ret;
}

Php::Value ampersand(Php::Parameters &params)
{
    unsigned char b[1];
    uint32_t pos = 0;
    serialize_byte(b, pos, (uint8_t)t_ampersand);
    return Php::Value((char*)b, pos);
}

Php::Value newline(Php::Parameters &params)
{
    unsigned char b[1];
    uint32_t pos = 0;
    serialize_byte(b, pos, (uint8_t)t_break);
    return Php::Value((char*)b, pos);
}