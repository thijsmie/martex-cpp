#pragma once

#include <phpcpp.h>

#include <map>
#include <set>
#include <vector>
#include <memory>
#include <string>

#include "core/language/transpiler.hpp"
#include "core/language/value.hpp"

class MarTeX : public Php::Base
{
  private:
    bool has_error;
    bool allow_page;

    Transpiler* t;
    std::string error_log;
    std::vector<std::string> modules;

  public:
    MarTeX();
    void RegisterModule(Php::Parameters &params);
    void Parse(Php::Parameters &params);
    void AllowPage();

    Php::Value HasError();
    Php::Value GetErrors();
    Php::Value GetResult();
    Php::Value GetPlainResult();
};