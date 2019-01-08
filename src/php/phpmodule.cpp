#include "phpmodule.hpp"
#include "phpvaluecasts.hpp"

PhpModule::PhpModule(std::string php_module_name) : myModule(php_module_name),
                                                    myEnvironmentNames(),
                                                    myEnvironments(),
                                                    myGlobals()
{
    Php::Value global_funcs = myModule.call("globals");
    /// Cast, fingers crossed
    myGlobals = global_funcs;
    for (const std::string &cmd : myGlobals)
    {
        callsigns[cmd] = {MoreFull};
        try
        {
            if (myModule.contains("argsfor_" + cmd))
                callsigns[cmd] = ParseSignature(myModule["argsfor_" + cmd]);
        }
        catch (Php::Exception &e)
        {
        }
    }

    Php::Value envs = myModule.call("environments");
    /// Cast, fingers crossed
    myEnvironments = envs;

    // put just the names in an array for easy returns
    for (auto it = myEnvironments.begin(); it != myEnvironments.end(); ++it)
    {
        myEnvironmentNames.push_back(it->first);
    }
}

void PhpModule::SetGlobal(Php::Object o) { GlobalEnv = o; }
std::vector<std::string> PhpModule::GetGlobals() { return myGlobals; }
std::vector<std::string> PhpModule::GetEnvs() { return myEnvironmentNames; };

std::shared_ptr<Environment> PhpModule::MakeEnv(std::string name, std::shared_ptr<Environment> parent)
{
    return std::make_shared<PhpEnvironment>(myEnvironments[name], myModule, GlobalEnv, parent);
}

Value PhpModule::RunGlobal(std::shared_ptr<Environment> env, Token name, std::vector<Value> &args)
{
    std::shared_ptr<PhpEnvironment> phpenv = std::dynamic_pointer_cast<PhpEnvironment>(env);
    try
    {
        Php::Value arguments = ValidateSignature(callsigns[name.GetLexeme()], args);
        if (phpenv != nullptr)
            return PhpToCpp(myModule.call(name.GetLexeme().c_str(), (Php::Value)phpenv->myEnvironment, arguments));
        else
            return PhpToCpp(myModule.call(name.GetLexeme().c_str(), (Php::Value)GlobalEnv, arguments));
    }
    catch (Php::Exception &exception)
    {
        throw RuntimeError(name, exception.message());
    }
}
