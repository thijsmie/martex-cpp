#include "phpenvironment.hpp"
#include <algorithm>
#include "phpvaluecasts.hpp"

PhpEnvironment::PhpEnvironment(std::string phpEnvName, Php::Object myModule, std::shared_ptr<Environment> parent) : myEnvironment(phpEnvName.c_str(), myModule)
{
    Php::Value funcs = myEnvironment.call("locals");
    /// Cast, fingers crossed
    commands = funcs;
    is_root = false;
    enclosing = parent;
}

bool PhpEnvironment::HasCommand(std::string c)
{
    return (std::find(commands.begin(), commands.end(), c) != commands.end());
}

Value PhpEnvironment::RunCommandHere(std::shared_ptr<Environment> env, Token name, std::vector<Value> args)
{
    std::shared_ptr<PhpEnvironment> phpenv = std::dynamic_pointer_cast<PhpEnvironment>(env);
    try
    {
        return PhpToCpp(myEnvironment.call(name.GetLexeme().c_str(), (Php::Value)phpenv->myEnvironment, CppToPhp(args)));
    }
    catch (Php::Exception &e)
    {
        throw RuntimeError(name, e.message());
    }
}

void PhpEnvironment::StartEnvironment(Token begin, Value arg)
{
    try
    {
        myEnvironment.call("begin", CppToPhp(arg));
    }
    catch (Php::Exception &e)
    {
        throw RuntimeError(begin, e.message());
    }
}

Value PhpEnvironment::EndEnvironment(Token end, Value content)
{
    try
    {
        return PhpToCpp(myEnvironment.call("end", CppToPhp(content)));
    }
    catch (Php::Exception &e)
    {
        throw RuntimeError(end, e.message());
    }
}