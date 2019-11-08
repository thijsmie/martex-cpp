#include "environment.hpp"

void Environment::Set(std::string name, Value value)
{
    values.insert(std::make_pair(name, std::move(value)));
}

void Environment::SetGlobal(std::string name, Value value)
{
    if (is_root)
    {
        auto i = values.find(name);
        if (i != values.end())
        {
            values.erase(i);
        }
        
        values.insert(std::make_pair(name, std::move(value)));
    }
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

void Environment::SetString(std::string name, std::string value)
{
    stringvalues.insert(std::make_pair(name, value));
}

void Environment::SetGlobalString(std::string name, std::string value)
{
    if (is_root)
        stringvalues.insert(std::make_pair(name, value));
    else
        enclosing->SetGlobalString(name, value);
}

std::string Environment::GetString(std::string name)
{
    if (stringvalues.find(name) == stringvalues.end())
    {
        if (is_root)
        {
            throw RuntimeError(Token(WORD, name, -1), "Undefined variable");
        }
        else
        {
            return enclosing->GetString(name);
        }
    }
    return stringvalues[name];
}

bool Environment::HasString(std::string name)
{
    if (stringvalues.find(name) == stringvalues.end())
    {
        if (is_root)
        {
            return false;
        }
        else
        {
            return enclosing->HasString(name);
        }
    }
    return true;
}

Value Environment::RunCommand(std::shared_ptr<Environment> env, Token name, std::vector<Value> arguments)
{
    if (HasCommand(name.GetLexeme()))
        return RunCommandHere(env, name, std::move(arguments));
    if (is_root)
        throw RuntimeError(name, "Unknown command.");
    return enclosing->RunCommand(env, name, std::move(arguments));
}