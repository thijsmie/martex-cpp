#pragma once

#include "language/error_reporter.hpp"
#include "language/interpreter.hpp"
#include "implementation/implementation.hpp"

#include <phpcpp.h>

#include <map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <string>

/// Base class for php environments to inherit
class PhpEnvironmentBase : public Php::Base
{
  public:
    void __construct(Php::Parameters &params)
    {
        Php::Value self(this);
        self["module"] = params[0];
    }
};

class MarTeX : public Php::Base
{
  private:
    bool has_error;
    std::string last_result;
    std::string error_log;
    std::vector<std::string> modules;
    void process_errors(ErrorReporter &reporter);

  public:
    MarTeX();
    void RegisterModule(Php::Parameters &params);
    void Parse(Php::Parameters &params);

    Php::Value HasError();
    Php::Value GetErrors();
    Php::Value GetResult();
};