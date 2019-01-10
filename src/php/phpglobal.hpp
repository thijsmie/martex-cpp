#pragma once

#include "implementation/implementation.hpp"
#include "phpvaluecasts.hpp"

#include <phpcpp.h>

#include <vector>
#include <map>
#include <string>
#include <memory>

class PhpGlobalEnvironment : public Php::Base//, public Php::ArrayAccess
{
  private:
    std::shared_ptr<GlobalEnv> gEnv;

  public:
    PhpGlobalEnvironment(std::shared_ptr<GlobalEnv> gEnv) : gEnv(gEnv) {}

    void __set(const Php::Value &name, const Php::Value &value)
    {
        std::string _name = name;
        gEnv->Set(_name, PhpToCpp(value));
    }

    Php::Value __get(const Php::Value &name)
    {
        std::string _name = name;
        return CppToPhpBytes(gEnv->Get(_name));
    }
};