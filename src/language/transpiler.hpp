#pragma once
    
#include "globalenvironment.hpp"
#include "error_reporter.hpp"
#include "interpreter.hpp"
    
#include "implementation/implementation.hpp"
    
#include <memory>
#include <vector>
    
class Transpiler
{
  private:
    ErrorReporter error_reporter;
    
  public:
    Transpiler(std::vector<std::shared_ptr<Module>>);
    Parse(std::string);
    
    void HadError();
    std::string GetErrors();
    std::string GetResult();
}