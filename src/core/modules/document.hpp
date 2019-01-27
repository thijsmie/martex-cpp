#include "core/language/environment.hpp"
#include <string>
#include <memory>

class ParagraphEnvironment : public Environment
{
  public:
    ParagraphEnvironment(std::shared_ptr<Environment> enclosing_)
    {
        enclosing = enclosing_;
        is_root = false;
    };

    bool HasCommand(std::string) { return false; }
    Value RunCommandHere(std::shared_ptr<Environment>, Token, std::vector<Value>) { return Value(); };
    void StartEnvironment(Token, Value){};
    Value EndEnvironment(Token, Value content)
    {
        return Value("p", Value::asString(content));
    }
};

class PageEnvironment : public Environment
{
  public:
    PageEnvironment(std::shared_ptr<Environment> enclosing_)
    {
        enclosing = enclosing_;
        is_root = false;
    }

    bool HasCommand(std::string cmd)
    {
        return (cmd == "link" || cmd == "meta");
    }

    Value RunCommandHere(std::shared_ptr<Environment>, Token cmd, std::vector<Value> arguments)
    {
        if (cmd.GetLexeme() == "link")
        {
            std::vector<Value> attr;
            attr.emplace_back("href", arguments[0].GetContent());
            attr.emplace_back("rel", arguments[1].GetContent());
            attr.emplace_back("type", arguments[2].GetContent());

            return Value("link", std::move(attr));
        }
        else
        {
            std::vector<Value> attr;
            attr.emplace_back("name", arguments[0].GetContent());
            attr.emplace_back("content", arguments[1].GetContent());

            return Value("meta", std::move(attr));
        }
    }

    void StartEnvironment(Token, Value){};
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

class DocumentEnvironment : public Environment
{
  public:
    DocumentEnvironment(std::shared_ptr<Environment> enclosing_)
    {
        enclosing = enclosing_;
        is_root = false;
    }

    bool HasCommand(std::string) { return false; }
    Value RunCommandHere(std::shared_ptr<Environment>, Token, std::vector<Value>) { return Value(); };
    void StartEnvironment(Token, Value){};
    Value EndEnvironment(Token, Value content)
    {
        return Value("body", Value::asString(content));
    }
};