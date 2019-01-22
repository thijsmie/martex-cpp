#include "globalenvironment.hpp"

GlobalEnv::GlobalEnv(std::vector<std::shared_ptr<Module>> modules) : commands()
{
    is_root = true;
    for (auto m : modules)
        for (auto c : m.get()->GetGlobals())
            commands[c] = m;
}

bool GlobalEnv::HasCommand(std::string c)
{
    return (commands.find(c) != commands.end());
}

Value GlobalEnv::RunCommandHere(std::shared_ptr<Environment> runenv, Token c, std::vector<Value> arguments)
{
    return commands[c.GetLexeme()].get()->RunGlobal(runenv, c, std::move(arguments));
}

void GlobalEnv::StartEnvironment(Token, Value) {}
Value GlobalEnv::EndEnvironment(Token, Value s) { return s; }