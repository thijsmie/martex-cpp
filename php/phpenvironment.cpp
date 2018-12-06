#include "phpenvironment.hpp"
#include <algorithm>
#include "phpvaluecasts.hpp"

PhpEnvironment::PhpEnvironment(std::string phpEnvName, Php::Object myModule): myEnvironment(phpEnvName.c_str(), myModule)
{
    Php::Value funcs = myModule.call("locals");
    /// Cast, fingers crossed
    commands = funcs;
}

bool PhpEnvironment::HasCommand(std::string c)
{
    return (std::find(commands.begin(), commands.end(), c) != commands.end());
}

Value PhpEnvironment::RunCommandHere(std::shared_ptr<Environment> env, std::string name, std::vector<Value> args)
{
    std::shared_ptr<PhpEnvironment> phpenv = std::dynamic_pointer_cast<PhpEnvironment>(env);
    return PhpToCpp(myEnvironment.call(name.c_str(), (Php::Value)phpenv->myEnvironment, CppToPhp(args)));
}

void PhpEnvironment::StartEnvironment(Value arg)
{
    myEnvironment.call("begin", myEnvironment, CppToPhp(arg));
}

Value PhpEnvironment::EndEnvironment(Value content)
{
    return PhpToCpp(myEnvironment.call("end", CppToPhp(content)));
}