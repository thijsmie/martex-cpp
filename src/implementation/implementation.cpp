#include "implementation.hpp"

Implementation::Implementation(std::vector<std::shared_ptr<Module>> modules) : modules(modules) {}

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
        temp = "&acute;";
        temp = temp.insert(1, 1, escapee);
        return temp;
    case TokenType::DUQUOT:
        temp = "&uml;";
        temp = temp.insert(1, 1, escapee);
        return temp;
    case TokenType::TICK:
        temp = "&grave;";
        temp = temp.insert(1, 1, escapee);
        return temp;
    case TokenType::TILT:
        temp = "&tilde;";
        temp = temp.insert(1, 1, escapee);
        return temp;
    case TokenType::HAT:
        temp = "&circ;";
        temp = temp.insert(1, 1, escapee);
        return temp;
    case TokenType::DASH:
        temp = "&slash;";
        temp = temp.insert(1, 1, escapee);
        return temp;
    case TokenType::DOT:
        temp = "&ring;";
        temp = temp.insert(1, 1, escapee);
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
            return "zzzzzz";
        }
    default:
        return "";
    }
}

std::shared_ptr<Environment> Implementation::Create(Token name, std::shared_ptr<Environment> parent)
{
    std::string cname = name.GetLexeme();

    for (auto m : modules)
        for (std::string e : m.get()->GetEnvs())
            if (e == cname)
                return m.get()->MakeEnv(cname, parent);

    throw RuntimeError(name, "No such environment.");
}
