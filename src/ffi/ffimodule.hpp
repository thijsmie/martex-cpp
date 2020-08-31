#pragma once

#include "core/language/module.hpp"
#include "ffienvironment.hpp"
#include "ffimartex.hpp"

#include <vector>
#include <map>
#include <string>


class FFIModule : public Module
{
  private:
    std::string name;
    MartexCtx* mycontext;
    std::map<std::string, std::vector<std::string>> myEnvironments;
    std::vector<std::string> myGlobals;
    int env_uniq;
    
  public:
    FFIModule(std::string, MartexCtx*);

    void AddGlobal(std::string);
    void AddEnvironment(std::string, std::vector<std::string>);

    std::vector<std::string> GetGlobals();
    std::vector<std::string> GetEnvs();

    std::shared_ptr<Environment> MakeEnv(std::string, std::shared_ptr<Environment>);
    Value RunGlobal(std::shared_ptr<Environment>, Token, std::vector<Value>);
};