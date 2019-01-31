#include "value.hpp"
#include <stdexcept>

void Value::SanitizeMulti()
{
    for (unsigned int i = 0; i < multicontent.size(); i++)
        if (multicontent[i].GetType() == t_multi)
        {
            // dissallow nested multi's
            std::string content = multicontent[i].GetContent();
            multicontent[i] = Value(t_string, content);
        }
}

Value Value::asString(const Value &v)
{
    return Value(t_string, v.GetContent());
}
Value Value::asString(const std::vector<Value> &vs)
{
    std::string ret = "";
    for (auto &v : vs)
    {
        ret += v.GetContent();
    }
    return Value(t_string, ret);
}

Value::Value(ValueType type) : type(type), content(), multicontent() {}

Value::Value(ValueType type, std::string content) : type(type), content(content) {}

Value::Value(ValueType type, std::string tag, std::string content) : type(type), tag(tag), content(content) {}

//multi
Value::Value(std::vector<Value> multicontent) : type(t_multi), multicontent(std::move(multicontent))
{
    SanitizeMulti();
}

//null
Value::Value() : type(t_null), content("") {}

//html
Value::Value(std::string tag, std::vector<Value> multicontent) : type(t_html), tag(tag), multicontent(std::move(multicontent))
{
    SanitizeMulti();
}

//info
Value::Value(ValueType type, std::string tag, std::vector<Value> multicontent) : type(type), tag(tag), multicontent(std::move(multicontent))
{
    SanitizeMulti();
}

Value::Value(std::string tag, Value cnt) : type(t_html), tag(tag), multicontent()
{
    multicontent.push_back(std::move(cnt));
    SanitizeMulti();
}

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
        for (const Value &v : multicontent)
            r_content += v.GetContent();
        return r_content;
    case t_html:
    {
        r_content = "\n<" + tag;

        bool hascontent = false;
        for (const Value &v : multicontent)
            if (v.GetType() == t_attr)
                r_content += " " + v.GetContent();
            else if (v.GetType() != t_null)
                hascontent = true;

        if (hascontent)
        {
            r_content += ">";
            for (const Value &v : multicontent)
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
    case t_info:
        return false;
    case t_multi:
    {
        for (const Value &v : multicontent)
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

std::string valueTypes[] = {
    "t_null",
    "t_string",
    "t_ampersand",
    "t_break",
    "t_multi",
    "t_html",
    "t_attr",
    "t_info"};

std::string ValueTypeName(ValueType v)
{
    return valueTypes[v];
}

// construct from bytes

Value::Value(const unsigned char *b, uint32_t &pos)
{
    uint8_t r_type = deserialize_byte(b, pos);
    type = (ValueType)(int)r_type;

    switch (r_type)
    {
    case t_null:
        content = "";
        break;
    case t_ampersand:
        content = "&amp;";
        break;
    case t_break:
        content = "<br/>";
        break;
    case t_string:
        content = deserialize_sized_string(b, pos);
        break;
    case t_attr:
        tag = deserialize_sized_string(b, pos);
        content = deserialize_sized_string(b, pos);
        break;
    case t_html:
    case t_info:
    {
        tag = deserialize_sized_string(b, pos);
        uint32_t content_length = deserialize_uint32_t(b, pos);
        uint32_t current_pos = pos;

        while (pos < current_pos + content_length)
            multicontent.push_back(Value(b, pos));
        break;
    }
    case t_multi:
    {
        uint32_t content_length = deserialize_uint32_t(b, pos);
        uint32_t current_pos = pos;

        while (pos < current_pos + content_length)
            multicontent.push_back(Value(b, pos));
        break;
    }
    }
}

void Value::WriteOut(unsigned char *b, uint32_t &pos) const
{
    serialize_byte(b, pos, (uint8_t)type);

    switch (type)
    {
    case t_null:
    case t_ampersand:
    case t_break:
        // no further action needed
        return;
    case t_string:
        serialize_sized_string(b, pos, content.c_str(), content.size());
        return;
    case t_attr:
        serialize_sized_string(b, pos, tag.c_str(), tag.size());
        serialize_sized_string(b, pos, content.c_str(), content.size());
        return;
    case t_html:
    case t_info:
    {
        serialize_sized_string(b, pos, tag.c_str(), tag.size());
        uint32_t content_length = 0;

        for (const Value &v : multicontent)
            content_length += v.ByteSize();

        serialize_uint32_t(b, pos, content_length);

        for (const Value &v : multicontent)
            v.WriteOut(b, pos);

        return;
    }
    case t_multi:
    {
        uint32_t content_length = 0;

        for (const Value &v : multicontent)
            content_length += v.ByteSize();

        serialize_uint32_t(b, pos, content_length);

        for (const Value &v : multicontent)
            v.WriteOut(b, pos);

        return;
    }
    }
}

uint32_t Value::ByteSize() const
{
    switch (type)
    {
    case t_null:
    case t_ampersand:
    case t_break:
        return sizeof(uint8_t);
    case t_string:
        return sizeof(uint8_t) + sizeof(uint32_t) + content.size();
    case t_attr:
        return sizeof(uint8_t) + 2 * sizeof(uint32_t) + tag.size() + content.size();
    case t_html:
    case t_info:
    {
        uint32_t content_length = 0;

        for (const Value &v : multicontent)
            content_length += v.ByteSize();

        return sizeof(uint8_t) + 2 * sizeof(uint32_t) + tag.size() + content_length;
    }
    case t_multi:
    {
        uint32_t content_length = 0;

        for (const Value &v : multicontent)
            content_length += v.ByteSize();

        return sizeof(uint8_t) + sizeof(uint32_t) + content_length;
    }
    }

    throw std::runtime_error("This valuetype is screwed up!");
}

Value Value::explicit_copy() const
{
    switch (type)
    {
    case t_null:
        return Value();
    case t_ampersand:
    case t_string:
    case t_break:
        return Value(type, content);
    case t_attr:
        return Value(tag, content);
    case t_html:
    case t_info:
    {
        std::vector<Value> copies;

        for (const Value &v : multicontent)
            copies.push_back(v.explicit_copy());

        return Value(type, tag, std::move(copies));
    }
    case t_multi:
    {
        std::vector<Value> copies;

        for (const Value &v : multicontent)
            copies.push_back(v.explicit_copy());

        return Value(std::move(copies));
    }
    }
    throw std::runtime_error("Weird valuetype!");
}