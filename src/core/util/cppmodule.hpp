#pragma once

#include "core/language/module.hpp"
#include "core/language/value.hpp"
#include "core/language/runtime_error.hpp"

#include <vector>
#include <map>
#include <string>
#include <algorithm>

namespace util
{

template <typename T> /// T is the implementing module
class CppModule : public Module
{
  private:
    typedef Value (T::*module_method_t)(std::shared_ptr<Environment>, Token, std::vector<Value>);
    typedef std::map<const std::string, module_method_t> module_method_map_t;
    module_method_map_t funcs;

  protected:
    void AddMethod(std::string name, module_method_t fn)
    {
        funcs[name] = fn;
    }

  public:
    std::vector<std::string> GetGlobals()
    {
        std::vector<std::string> funcnames;
        funcnames.reserve(funcs.size());
        for (auto const &kv : funcs)
            funcnames.push_back(kv.first);

        return std::move(funcnames);
    }

    Value RunGlobal(std::shared_ptr<Environment> env, Token cmd, std::vector<Value> args)
    {
        auto x = funcs.find(cmd.GetLexeme());

        if (x != funcs.end())
        {
            /// So okay, this is complicated:
            // (T*)this: convert the base class pointer to a derived class pointer
            // ->*(x->second): get the method address from the map and call it
            return (((T *)this)->*(x->second))(env, cmd, std::move(args));
        }
        throw RuntimeError(cmd, "Command does not exist but should, internal error");
    }
};

} // namespace util