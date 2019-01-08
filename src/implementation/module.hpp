#pragma once

#include <string>
#include <memory>
#include <vector>

#include "language/environment.hpp"

class Module
{
  public:
    virtual std::vector<std::string> GetGlobals() = 0;
    virtual std::vector<std::string> GetEnvs() = 0;
    virtual std::shared_ptr<Environment> MakeEnv(std::string, std::shared_ptr<Environment>) = 0;
    virtual Value RunGlobal(std::shared_ptr<Environment>, Token, std::vector<Value>&) = 0;
};