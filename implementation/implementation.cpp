#include "implementation.hpp"

Implementation::Implementation(std::vector<std::shared_ptr<Module>> modules) : modules(modules) {}

std::shared_ptr<Environment> Implementation::Global()
{
    return std::make_shared<GlobalEnv>(modules);
}

std::string Implementation::LineBreak() const
{
    return "<br />";
}

std::string Implementation::Ampersand() const
{
    return "&amp;";
}

std::string Implementation::Escaped(TokenType escapetype, char escapee)
{
    std::string temp;
    switch (escapetype)
    {
    case TokenType::QUOT:
        temp = "& acute;";
        temp.insert(1, 1, escapee);
        return temp;
    case TokenType::DUQUOT:
        temp = "& uml;";
        temp.insert(1, 1, escapee);
        return temp;
    case TokenType::TICK:
        temp = "& grave;";
        temp.insert(1, 1, escapee);
        return temp;
    case TokenType::TILT:
        temp = "& tilde;";
        temp.insert(1, 1, escapee);
        return temp;
    case TokenType::HAT:
        temp = "& circ;";
        temp.insert(1, 1, escapee);
        return temp;
    case TokenType::DASH:
        temp = "& uml;";
        temp.insert(1, 1, escapee);
        return temp;
    case TokenType::DOT:
        temp = "& ring;";
        temp.insert(1, 1, escapee);
        return temp;
    case TokenType::ESCAPE:
        switch (escapee)
        {
        case '%':
            return "%";
        case '{':
            return "&#123;";
        case '}':
            return "&#125;";
        case '<':
            return "&lt;";
        case '>':
            return "&gt;";
        case '&':
            return "&amp;";
        default:
            return "";
        }
    default:
        return "";
    }
}

std::shared_ptr<Environment> Implementation::Create(Token name, std::shared_ptr<Environment> parent)
{
    std::string cname = name.ToString();

    for (auto m : modules)
        for (std::string e : m.get()->GetEnvs())
            if (e == cname)
                return m.get()->MakeEnv(cname, parent);

    throw new RuntimeError(name, "No such environment.");
}

GlobalEnv::GlobalEnv(std::vector<std::shared_ptr<Module>> modules) : commands()
{
    for (auto m : modules)
        for (auto c : m.get()->GetGlobals())
            commands[c] = m;
}

bool GlobalEnv::HasCommand(std::string c)
{
    return (commands.find(c) == commands.end());
}

Value GlobalEnv::RunCommandHere(std::shared_ptr<Environment> runenv, std::string c, std::vector<Value> arguments)
{
    return commands[c].get()->RunGlobal(runenv, c, arguments);
}

void GlobalEnv::StartEnvironment(Value) {}
Value GlobalEnv::EndEnvironment(Value s) { return s; }