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

    Php::Value envs = myModule.call("environments");
    /// Cast, fingers crossed
    myEnvironments = envs;

    // put just the names in an array for easy returns
    for (auto it = myEnvironments.begin(); it != myEnvironments.end(); ++it)
    {
        myEnvironmentNames.push_back(it->first);
    }
}

std::vector<std::string> GetGlobals() { return myGlobals; }
std::vector<std::string> GetEnvs() { return myEnvironmentNames; };

std::shared_ptr<Environment> MakeEnv(std::string name, std::shared_ptr<Environment> parent)
{
    return make_shared(PhpEnvironment(myEnvironments[name], myModule));
}

Value RunGlobal(std::shared_ptr<Environment> env, std::string name, std::vector<Value> args)
{
    std::shared_ptr<PhpEnvironment> phpenv = std::dynamic_pointer_cast<PhpEnvironment>(env);
    return PhpToCpp(myModule.call(name, phpenv->myEnvironment, CppToPhp(args)));
}
