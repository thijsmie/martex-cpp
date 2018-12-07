#pragma once

#include "language/environment.hpp"

#include <phpcpp.h>

#include <vector>
#include <map>
#include <string>

class PhpEnvironment : public Environment
{
  private:
    std::vector<std::string> commands;
  public:
    Php::Object myEnvironment;

    PhpEnvironment(std::string, Php::Object);
    bool HasCommand(std::string);
    Value RunCommandHere(std::shared_ptr<Environment>, Token, std::vector<Value>);
    void StartEnvironment(Token, Value);
    Value EndEnvironment(Token, Value);
};