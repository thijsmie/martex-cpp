#pragma once

#include <phpcpp.h>

#include <map>
#include <set>
#include <vector>
#include <memory>
#include <string>

class MarTeX : public Php::Base
{
  private:
    bool has_error;
    bool allow_page;
    std::string last_result;
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
};