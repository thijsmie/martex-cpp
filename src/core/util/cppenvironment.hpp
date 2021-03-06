#pragma once

#include "core/language/environment.hpp"
#include "core/language/value.hpp"
#include "core/language/runtime_error.hpp"

#include <vector>
#include <map>
#include <string>
#include <algorithm>

namespace util
{

template <typename T> /// T is the implementing environment
class CppEnvironment : public Environment
{
  private:
    typedef Value (T::*module_method_t)(Token, std::vector<Value>);
    typedef std::map<const std::string, module_method_t> module_method_map_t;
    module_method_map_t funcs;

  protected:
    void AddMethod(std::string name, module_method_t fn)
    {
        funcs[name] = fn;
    }

  public:
    CppEnvironment(std::shared_ptr<Environment> p) : Environment(p) {}
    
    bool HasCommand(std::string name)
    {
        return (funcs.find(name) != funcs.end());
    }

    Value RunCommandHere(std::shared_ptr<Environment> env, Token cmd, std::vector<Value> args)
    {
        if (env.get() != (Environment *)this)
            throw RuntimeError(cmd, "cannot call in nested environment");

        auto x = funcs.find(cmd.GetLexeme());

        if (x != funcs.end())
        {
            /// So okay, this is complicated:
            // (T*)this: convert the base class pointer to a derived class pointer
            // ->*(x->second): get the method address from the map and call it
            return (((T *)this)->*(x->second))(cmd, std::move(args));
        }
        throw RuntimeError(cmd, "Command does not exist but should, internal error");
    }
};

} // namespace util