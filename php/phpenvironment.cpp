#include "phpenvironment.hpp"


PhpEnvironment::PhpEnvironment(std::string phpEnvName, Php::Object myModule): myEnvironment(phpEnvName, myModule)
{
    Php::Value funcs = myModule.call("locals");
    /// Cast, fingers crossed
    commands = funcs;
}

bool PhpEnvironment::HasCommand(std::string c)
{
    return (commands.find(c) != commands.end());
}

Value PhpEnvironment::RunCommandHere(std::shared_ptr<Environment> env, std::string name, std::vector<Value> args)
{
    std::shared_ptr<PhpEnvironment> phpenv = std::dynamic_pointer_cast<PhpEnvironment>(env);
    return PhpToCpp(myModule.call(name, phpenv->myEnvironment, CppToPhp(args)));
}

void PhpEnvironment::StartEnvironment(Value)
{
    return PhpToCpp(myModule.call("begin", myEnvironment, CppToPhp(args)));
}

void PhpEnvironment::EndEnvironment(Value)
{
    std::shared_ptr<PhpEnvironment> phpenv = std::dynamic_pointer_cast<PhpEnvironment>(env);
    return PhpToCpp(myModule.call("end", myEnvironment, CppToPhp(args)));
}