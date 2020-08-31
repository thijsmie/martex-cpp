#pragma once

#include "core/language/environment.hpp"
#include "ffimartex.hpp"

#include <vector>
#include <map>
#include <string>
#include <memory>


class FFIEnvironment : public Environment
{
  private:
    MartexCtx* mycontext;
    std::vector<std::string> commands;

  public:
    std::string envname;
    int identifier;

    FFIEnvironment(MartexCtx*, std::string, int, std::vector<std::string>, std::shared_ptr<Environment>);
    bool HasCommand(std::string);
    Value RunCommandHere(std::shared_ptr<Environment>, Token, std::vector<Value>);
    void StartEnvironment(Token, Value);
    Value EndEnvironment(Token, Value);
};
