#pragma once
    
#include "error_reporter.hpp"
#include "module.hpp"
    
#include <memory>
#include <vector>
    
class Transpiler
{
  private:
    bool used;
    ErrorReporter error_reporter;
    std::vector<std::shared_ptr<Module>> modules;
    Value result;
    
  public:
    Transpiler(std::vector<std::shared_ptr<Module>>);

    void Parse(std::string);
    
    bool HasError();
    std::string GetErrors();
    std::string GetResult();
    const Value& GetResultRaw();
    std::string GetResultBytes();
};