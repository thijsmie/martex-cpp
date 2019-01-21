#pragma once

#include "language/module.hpp"
#include "language/value.hpp"
#include "language/runtime_error.hpp"

#include <vector>
#include <map>
#include <string>
#include <algorithm>

template<typename T> /// T is the implementing module
class CppModule : public Module
{
  private:
    typedef Value (T::*module_method_t)(std::shared_ptr<Environment>, Token, std::vector<Value>);
    typedef std::map<const std::string, derived_method_t> module_method_map_t;
    
    module_method_map_t funcs;
  
  protected:
    void AddMethod(std::string name, module_method_t fn);
    
  public:
    std::vector<std::string> GetGlobals();
    Value RunGlobal(std::shared_ptr<Environment>, Token, std::vector<Value>);
};