#include "environment.hpp"


Environment::Environment() : is_root(true), values(), enclosing(nullptr) {}

Environment::Environment(std::string name, std::shared_ptr<Environment> encl) : is_root(false), values(), enclosing(encl) {}

void Environment::Set(std::string name, Value value)
{
    values[name] = value;
}

Value Environment::Get(Token name)
{
    if (!values.contains(name->GetContent()))
    {
        if (enclosing == nullptr)
        {
            throw RuntimeError(name, "Undefined variable " + name.GetContent() + ".");
        }
        else
        {
            return enclosing->Get(name);
        }
    }
    return values[name];
}

Value Environment::RunCommand(Token name, std::vector<Value> arguments)
{
    return RunCommand(std::make_shared<Environment>(this), name, arguments);
}

Value Environment::RunCommand(std::shared_ptr<Environment> env, Token name, std::vector<Value> arguments)
{
    if (HasCommand(name))
        return RunCommandHere(env, name->GetContent(), arguments);
    if (is_root)
        throw RuntimeError(name, "Unknown command " + name.GetContent() + ".");
    return enclosing->RunCommand(name, arguments);
}