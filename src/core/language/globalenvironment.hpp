#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>

#include "environment.hpp"
#include "token.hpp"
#include "module.hpp"

class GlobalEnv : public Environment
{
  private:
    std::map<std::string, std::shared_ptr<Module>> commands;
    
  public:
    GlobalEnv(std::vector<std::shared_ptr<Module>>);
    
    bool HasCommand(std::string);
    Value RunCommandHere(std::shared_ptr<Environment>, Token, std::vector<Value>);
    void StartEnvironment(Token, Value);
    Value EndEnvironment(Token, Value);
};