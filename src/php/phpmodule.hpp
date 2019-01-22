#pragma once

#include "core/language/module.hpp"
#include "phpenvironment.hpp"

#include <phpcpp.h>

#include <vector>
#include <map>
#include <string>

/// Base class for php modules to inherit
class PhpModuleBase : public Php::Base
{
public:
  Php::Value globals()
  {
    return Php::Array();
  }

  Php::Value environments()
  {
    return Php::Array();
  }
};

class PhpModule : public Module
{
  private:
    Php::Object GlobalEnv;
    Php::Object myModule;
    std::vector<std::string> myEnvironmentNames;
    std::map<std::string, std::string> myEnvironments;
    std::vector<std::string> myGlobals;
    std::map<std::string, std::vector<ArgType>> callsigns;
    
  public:
    PhpModule(std::string);

    void SetGlobal(Php::Object);

    std::vector<std::string> GetGlobals();
    std::vector<std::string> GetEnvs();
    std::shared_ptr<Environment> MakeEnv(std::string, std::shared_ptr<Environment>);
    Value RunGlobal(std::shared_ptr<Environment>, Token, std::vector<Value>);
};