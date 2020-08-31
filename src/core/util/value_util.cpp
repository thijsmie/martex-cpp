#include "value_util.hpp"
#include "string_util.hpp"

namespace util
{

void ltrimws(std::vector<Value> &values)
{
    if (values.size() == 0)
        return;

    Value *v = &values[0];
    while (v->GetType() == t_null || (v->GetType() == t_string && wsonly(v->GetContent())))
    {
        values.erase(values.begin());

        if (values.size() == 0)
            return;

        v = &values[0];
    }

    if (v->GetType() == t_string)
        values[0] = Value(t_string, ltrim_copy(v->GetContent()));
}

void rtrimws(std::vector<Value> &values)
{
    if (values.size() == 0)
        return;

    Value *v = &values[values.size() - 1];
    while (v->GetType() == t_null || (v->GetType() == t_string && wsonly(v->GetContent())))
    {
        values.erase(values.end() - 1);

        if (values.size() == 0)
            return;

        v = &values[values.size() - 1];
    }

    if (v->GetType() == t_string)
        values[values.size()-1] = Value(t_string, rtrim_copy(v->GetContent()));
}

void trimws(std::vector<Value> &values)
{
    rtrimws(values);
    ltrimws(values);
}

} // namespace util