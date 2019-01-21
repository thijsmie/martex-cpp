#include "cppmodule.hpp"

/// protected methods for method registration
void CppModule::AddMethod(std::string name, module_method_t fn)
{
    funcs[name] = fn;
}

/// virtual methods from Module
std::vector<std::string> CppModule::GetGlobals();
{
    std::vector<std::string> funcnames;
    funcnames.reserve(funcs.size());
    for (auto const& kv : funcs)
        funcnames.push_back(kv.first);
    
    return std::move(funcnames);
}

Value CppModule::RunGlobal(std::shared_ptr<Environment> env, Token cmd, std::vector<Value> args)
{
    auto x = funcs.find(cmd.GetLexeme());

    if (x != funcs.end()) {
        return (((T*)this)->*(x->second))(env, cmd, std::move(args));
    }
    throw RuntimeError(cmd, "Command does not exist but should, internal error");
}