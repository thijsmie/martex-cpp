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
    Value RunCommandHere(std::shared_ptr<Environment>, Token, std::vector<Value>&) { return Value(); };
    void StartEnvironment(Token, Value&){};
    Value EndEnvironment(Token, Value &content)
    {
        return Value("p", {Value(t_string, content.GetContent())});
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
    Value RunCommandHere(std::shared_ptr<Environment>, Token, std::vector<Value>&) { return Value(); };
    void StartEnvironment(Token, Value&){};
    Value EndEnvironment(Token, Value &content)
    {
        return Value({Value(t_string, "<!DOCTYPE html>"), Value("html", {Value(t_string, content.GetContent())})});
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
    Value RunCommandHere(std::shared_ptr<Environment>, Token, std::vector<Value>&) { return Value(); };
    void StartEnvironment(Token, Value&){};
    Value EndEnvironment(Token, Value& content)
    {
        return Value("body", {Value(t_string, content.GetContent())});
    }
};