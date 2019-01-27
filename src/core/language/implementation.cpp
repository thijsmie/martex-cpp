#include "implementation.hpp"

std::string Implementation::LineBreak()
{
    return "<br />";
}

std::string Implementation::Ampersand()
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
