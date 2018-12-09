#include "environment.hpp"

void Environment::Set(std::string name, Value value)
{
    values[name] = value;
}

Value Environment::Get(Token name)
{
    if (values.find(name.GetLexeme()) == values.end())
    {
        if (enclosing == nullptr)
        {
            throw RuntimeError(name, "Undefined variable " + name.GetLexeme() + ".");
        }
        else
        {
            return enclosing->Get(name);
        }
    }
    return values[name.GetLexeme()];
}


Value Environment::RunCommand(std::shared_ptr<Environment> env, Token name, std::vector<Value> arguments)
{
    if (HasCommand(name.GetLexeme()))
        return RunCommandHere(env, name, arguments);
    if (is_root)
        throw RuntimeError(name, "Unknown command.");
    return enclosing->RunCommand(env, name, arguments);
}