#pragma once

#include "implementation/module.hpp"
#include "phpenvironment.hpp"

#include <phpcpp.h>

#include <vector>
#include <map>
#include <string>

class PhpModule : public Module
{
  private:
    Php::Object myModule;
    std::map<std::string, std::string> myEnvironments;
    std::vector<std::string> myGlobals;
    std::vector<std::string> myEnvironmentNames;
    
  public:
    PhpModule(std::string);

    std::vector<std::string> GetGlobals();
    std::vector<std::string> GetEnvs();
    std::shared_ptr<Environment> MakeEnv(std::string, std::shared_ptr<Environment>);
    Value RunGlobal(std::shared_ptr<Environment>, std::string, std::vector<Value>);
};