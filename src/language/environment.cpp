#include "environment.hpp"

void Environment::Set(std::string name, Value value)
{
    values.insert(std::make_pair(name, std::move(value)));
}

void Environment::SetGlobal(std::string name, Value value)
{
    if (is_root)
        values.insert(std::make_pair(name, std::move(value)));
    else
        enclosing->SetGlobal(name, std::move(value));
}

Value& Environment::Get(Token name)
{
    if (values.find(name.GetLexeme()) == values.end())
    {
        if (is_root)
        {
            throw RuntimeError(name, "Undefined variable");
        }
        else
        {
            return enclosing->Get(name);
        }
    }
    return values[name.GetLexeme()];
}

Value& Environment::Get(std::string name)
{
    if (values.find(name) == values.end())
    {
        if (is_root)
        {
            throw RuntimeError(Token(WORD, name, -1), "Undefined variable");
        }
        else
        {
            return enclosing->Get(name);
        }
    }
    return values[name];
}


Value Environment::RunCommand(std::shared_ptr<Environment> env, Token name, std::vector<Value> arguments)
{
    if (HasCommand(name.GetLexeme()))
        return RunCommandHere(env, name, std::move(arguments));
    if (is_root)
        throw RuntimeError(name, "Unknown command.");
    return enclosing->RunCommand(env, name, std::move(arguments));
}