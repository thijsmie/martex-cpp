#include "core/language/environment.hpp"
#include "module.hpp"
#include <string>
#include <memory>

class FigureModule : public Module
{

};

class FigureEnvironment : public Environment
{
  private:
    const std::string commands[5] = {
        "includegraphics", "width", "height", "caption", "label"
    };

    std::string caption;

  public:
    FigureEnvironment(std::shared_ptr<Environment> enclosing_)
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
