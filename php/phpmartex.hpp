#pragma once

#include "language/error_reporter.hpp"
#include "language/interpreter.hpp"
#include "implementation/implementation.hpp"
#include "phpenvironment.hpp"

#include <phpcpp.h>

#include <map>
#include <set>
#include <vector>
#include <memory>
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

/// Base class for php environments to inherit
class PhpEnvironmentBase : public Php::Base
{
private:
  std::shared_ptr<PhpEnvironment> RealMe;

public:
  void __construct(Php::Parameters &params)
  {
    Php::Value self(this);
    self["module"] = params[0];
    self["tex"] = params[1];
  }

  Php::Value locals()
  {
    return Php::Array();
  }

  void begin(Php::Parameters &params)
  {
  }

  Php::Value end(Php::Parameters &params)
  {
    return Php::Value();
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