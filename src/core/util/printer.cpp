#include "printer.hpp"

void Print(const Value &v, int indent)
{
    std::cerr << std::string(indent, ' ');
    std::cerr << ValueTypeName(v.GetType());

    switch (v.GetType())
    {
    case t_null:
    case t_ampersand:
    case t_break:
        std::cerr << std::endl;
        break;
    case t_string:
        std::cerr << ' ' << v.GetContent() << std::endl;
        break;
    case t_multi:
        std::cerr << std::endl;
        for (const Value &vi : v.multicontent)
            Print(vi, indent + 2);
        break;
    case t_info:
    case t_html:
        std::cerr << ' ' << v.GetTag() << std::endl;
        for (const Value &vi : v.multicontent)
            Print(vi, indent + 2);
        break;
    case t_attr:
        std::cerr << ' ' << v.GetContent() << std::endl;
        break;
    }
}

void Print(const Value &v)
{
    Print(v, 0);
}

void PrintToplevelI(const Value &v)
{
    std::cerr << ValueTypeName(v.GetType());

    switch (v.GetType())
    {
    case t_null:
    case t_ampersand:
    case t_break:
    case t_multi:
        std::cerr << std::endl;
        break;
    case t_string:
        if (v.GetContent() == "\n")
            std::cerr << " <LINE>" << std::endl;
        else
            std::cerr << ' ' << v.GetContent() << std::endl;
        break;
    case t_info:
    case t_html:
        std::cerr << ' ' << v.GetTag() << std::endl;
        break;
    case t_attr:
        std::cerr << ' ' << v.GetContent() << std::endl;
        break;
    }
}

void PrintToplevel(const Value &v)
{
    for (const Value &vi : v.multicontent)
        PrintToplevelI(vi);
}