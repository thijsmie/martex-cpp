#pragma once

#include "core/util/cppenvironment.hpp"
#include <string>
#include <memory>

class ParagraphEnvironment : public util::CppEnvironment<ParagraphEnvironment>
{
  public:
    ParagraphEnvironment(std::shared_ptr<Environment> parent) : util::CppEnvironment<ParagraphEnvironment>(parent)
    {
    }

    void StartEnvironment(Token, Value) {}
    Value EndEnvironment(Token, Value content)
    {
        return Value("p", Value::asString(content));
    }
};

class CodeEnvironment : public util::CppEnvironment<CodeEnvironment>
{
  public:
    CodeEnvironment(std::shared_ptr<Environment> parent) : util::CppEnvironment<CodeEnvironment>(parent)
    {
    }

    void StartEnvironment(Token, Value) {}
    Value EndEnvironment(Token, Value content)
    {
        content.multicontent.emplace(content.multicontent.begin(), "class", "martex-mono");

        return Value("pre", std::move(content.multicontent));
    }
};