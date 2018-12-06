#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

#include "language/environment.hpp"
#include "language/token.hpp"
#include "implementation/module.hpp"


class GlobalEnv : public Environment
{
  private:
    std::unordered_map<std::string, std::shared_ptr<Module>> commands;
  public:
    GlobalEnv(std::vector<std::shared_ptr<Module>>);
    bool HasCommand(std::string);
    Value RunCommandHere(std::shared_ptr<Environment>, std::string, std::vector<Value>);
    void StartEnvironment(Value);
    void EndEnvironment(Value);
};

class Implementation
{
  private:
    std::vector<std::shared_ptr<Module>> modules;
  public:
    Implementation(std::vector<std::shared_ptr<Module>>);
    std::shared_ptr<Environment> Global();
    std::shared_ptr<Environment> Create(Token name, std::shared_ptr<Environment> parent);
    std::string Escaped(TokenType escapetype, char escapee);
    std::string LineBreak() const;
    std::string Ampersand() const;
};
