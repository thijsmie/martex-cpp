#pragma once

#include "core/util/cppenvironment.hpp"
#include "core/util/value_util.hpp"
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
  private:
    std::string language;

  public:
    void StartEnvironment(Token, Value v)
    {
        if (v.GetContent() != "" && v.IsPlain())
            language = v.GetContent();
    }

    Value EndEnvironment(Token, Value content)
    {
        std::vector<Value> &cnt = content.multicontent;
        util::trimws(cnt);

        cnt.emplace(cnt.begin(), "class", "language-" + language);

        return Value("pre", Value("code", std::move(cnt)));
    }

    Value command(Token, std::vector<Value> args)
    {
        /// normal commands screws with the markup, override in local env
        if (args.size() == 1)
        {
            args.emplace(args.begin(), t_string, "&#92;");
            return Value(std::move(args));
        }
        else
        {
            args.reserve(args.size() + (args.size() - 1) * 2 + 1);

            for (int i = args.size() - 1; i > 0; i--)
            {
                args.emplace(args.begin() + i + 1, t_string, "&#125;");
                args.emplace(args.begin() + i, t_string, "&#123;");
            }

            args.emplace(args.begin(), t_string, "&#92;");
            return Value(std::move(args));
        }
    }

    CodeEnvironment(std::shared_ptr<Environment> parent) : util::CppEnvironment<CodeEnvironment>(parent)
    {
        AddMethod("command", &CodeEnvironment::command);
    }
};

class CenterEnvironment : public util::CppEnvironment<CenterEnvironment>
{
  public:
    CenterEnvironment(std::shared_ptr<Environment> parent) : util::CppEnvironment<CenterEnvironment>(parent)
    {
    }

    void StartEnvironment(Token, Value) {}
    Value EndEnvironment(Token, Value content)
    {
        content.multicontent.emplace(content.multicontent.begin(), "style", "text-align: center;");

        return Value("div", std::move(content.multicontent));
    }
};