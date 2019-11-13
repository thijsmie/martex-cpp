#pragma once

#include "core/util/cppenvironment.hpp"
#include "core/language/runtime_error.hpp"
#include <string>
#include <memory>

class PageEnvironment : public util::CppEnvironment<PageEnvironment>
{
  public:
    PageEnvironment(std::shared_ptr<Environment> parent) : util::CppEnvironment<PageEnvironment>(parent)
    {
        AddMethod("link", &PageEnvironment::link);
        AddMethod("meta", &PageEnvironment::meta);
        AddMethod("script", &PageEnvironment::script);
        AddMethod("script_inline", &PageEnvironment::script_inline);
    }

    Value link(Token cmd, std::vector<Value> arguments)
    {
        if (arguments.size() != 3)
            throw RuntimeError(cmd, "takes three arguments");
            
        std::vector<Value> attr;
        attr.emplace_back("href", arguments[0].GetContent());
        attr.emplace_back("rel", arguments[1].GetContent());
        attr.emplace_back("type", arguments[2].GetContent());

        return Value("link", std::move(attr));
    }

    Value meta(Token cmd, std::vector<Value> arguments)
    {
        if (arguments.size() != 2)
            throw RuntimeError(cmd, "takes two arguments");

        std::vector<Value> attr;
        attr.emplace_back("name", arguments[0].GetContent());
        attr.emplace_back("content", arguments[1].GetContent());

        return Value("meta", std::move(attr));
    }

    Value script(Token cmd, std::vector<Value> arguments)
    {

        if (arguments.size() != 1)
            throw RuntimeError(cmd, "takes one argument");

        std::vector<Value> attr;
        attr.emplace_back("src", arguments[0].GetContent());
        attr.emplace_back(t_string, "");
        return Value("script", std::move(attr));
    }

    Value script_inline(Token, std::vector<Value> arguments)
    {
        return Value("script", std::move(arguments));
    }

    void StartEnvironment(Token, Value) {}
    Value EndEnvironment(Token, Value content)
    {
        std::vector<Value> ret;

        std::vector<Value> html;
        std::vector<Value> head;

        for (Value &v : content.multicontent)
        {
            if (v.GetType() == t_html && v.GetTag() == "body")
            {
                html.push_back(std::move(v));
                break;
            }
            else
            {
                head.push_back(std::move(v));
            }
        }

        html.emplace(html.begin(), "head", std::move(head));

        ret.push_back(Value(t_string, "<!DOCTYPE html>"));
        ret.push_back(Value("html", std::move(html)));
        return Value(std::move(ret));
    }
};