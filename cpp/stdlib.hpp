#pragma once

#include "implementation/module.hpp"

#include <vector>
#include <map>
#include <string>
#include <algorithm>

class StdLib : public Module
{
    
  public:
    StdLib();

    std::vector<std::string> GetGlobals();
    std::vector<std::string> GetEnvs();
    std::shared_ptr<Environment> MakeEnv(std::string, std::shared_ptr<Environment>);
    Value RunGlobal(std::shared_ptr<Environment>, Token, std::vector<Value>);
};