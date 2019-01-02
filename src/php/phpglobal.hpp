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
        Value v = PhpToCpp(value);
        gEnv->Set(_name, v);
    }

    Php::Value __get(const Php::Value &name)
    {
        std::string _name = name;
        Value v = gEnv->Get(_name);
        return CppToPhp(v);
    }
};