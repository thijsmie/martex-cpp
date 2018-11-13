#include "value.hpp"

Value::Value(ValueType type, std::string content) : type(type), content(content), multicontent(nullptr) {}
Value::Value(std::vector<Value> multicontent) : type(t_multi), multicontent(multicontent)
{
    content = "";
    for (Value v : multicontent)
        content += v.GetContent();
}
Value::Value() : type(t_null), content(""), multicontent(nullptr) {}

Value::GetContent() { return content; }
Value::GetType() { return type; }
Value::GetValues() { return multicontent; }