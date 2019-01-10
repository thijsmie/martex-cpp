#include "language/environment.hpp"
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

    bool HasCommand(std::string) { return false; }
    Value RunCommandHere(std::shared_ptr<Environment>, Token, std::vector<Value>) { return Value(); };
    void StartEnvironment(Token, Value){};
    Value EndEnvironment(Token, Value content)
    {
        std::vector<Value> ret; ret.reserve(2);
        ret.push_back(Value(t_string, "<!DOCTYPE html>"));
        ret.push_back(Value("html", Value::asString(content)));
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