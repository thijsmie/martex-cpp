#pragma once

#include "core/util/cppenvironment.hpp"
#include <string>
#include <memory>

class PageEnvironment : public util::CppEnvironment<PageEnvironment>
{
  public:
    PageEnvironment(std::shared_ptr<Environment> parent) : util::CppEnvironment<PageEnvironment>(parent)
    {
        AddMethod("link", &PageEnvironment::link);
        AddMethod("meta", &PageEnvironment::meta);
    }

    Value link(Token, std::vector<Value> arguments)
    {
        std::vector<Value> attr;
        attr.emplace_back("href", arguments[0].GetContent());
        attr.emplace_back("rel", arguments[1].GetContent());
        attr.emplace_back("type", arguments[2].GetContent());

        return Value("link", std::move(attr));
    }

    Value meta(Token, std::vector<Value> arguments)
    {
        std::vector<Value> attr;
        attr.emplace_back("name", arguments[0].GetContent());
        attr.emplace_back("content", arguments[1].GetContent());

        return Value("meta", std::move(attr));
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
        ret.push_back(Value("html", Value::asString(html)));
        return Value(std::move(ret));
    }
};