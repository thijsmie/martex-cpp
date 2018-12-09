#pragma once

#include <memory>
#include <map>
#include "runtime_error.hpp"
#include "value.hpp"
#include "token.hpp"

class Environment
{
  protected:
    bool is_root;
    std::map<std::string, Value> values;
    std::shared_ptr<Environment> enclosing;

  public:
    void Set(std::string, Value);
    Value Get(Token);
    Value RunCommand(std::shared_ptr<Environment>, Token, std::vector<Value>);

    virtual bool HasCommand(std::string) = 0;
    virtual Value RunCommandHere(std::shared_ptr<Environment>, Token, std::vector<Value>) = 0;
    virtual void StartEnvironment(Token, Value) = 0;
    virtual Value EndEnvironment(Token, Value) = 0;
};
