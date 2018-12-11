#include "value.hpp"

Value::Value(ValueType type, std::string content) : type(type), content(content) {}
Value::Value(std::vector<Value> multicontent) : type(t_multi), multicontent(multicontent)
{
    content = "";
    for (Value v : multicontent)
        content += v.GetContent();
}
Value::Value(std::initializer_list<Value> multicontent) : type(t_multi), multicontent(multicontent)
{
    content = "";
    for (Value v : multicontent)
        content += v.GetContent();
}
Value::Value() : type(t_null), content("") {}

std::string Value::GetContent() const { return content; }
ValueType Value::GetType() const { return type; }
std::vector<Value> Value::GetValues() const { return multicontent; }

Value Value::Flattened() 
{
    if (type != t_multi)
        return Value(std::vector<Value>({*this}));

    std::vector<Value> ret;

    for(Value v : multicontent)
    {
        std::vector<Value> flat = v.Flattened().GetValues();
        ret.insert(ret.end(), flat.begin(), flat.end() );
    }

    return Value(ret);
}