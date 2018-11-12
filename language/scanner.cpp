#include "scanner.hpp"

using std::string;
using std::vector;

bool Scanner::IsAtEnd() { return current >= (int)source.size(); }

void Scanner::AddToken(TokenType type, string lexeme)
{
    tokens.push_back(Token(type, lexeme, line));
}

void Scanner::AddToken(TokenType type)
{
    string lexeme = source.substr(start, current - start);
    AddToken(type, lexeme);
}

char Scanner::Peek() { return IsAtEnd() ? '\0' : source[current]; }

char Scanner::PeekNext()
{
    return current + 1 >= (int)source.size() ? '\0' : source[current + 1];
}

char Scanner::Advance() { return source[current++]; }

bool Scanner::Match(char c)
{
    if (!IsAtEnd() && Peek() == c)
    {
        Advance();
        return true;
    }
    return false;
}

void Scanner::EatWS()
{
    while (Peek() == " " || Peek() == "\t" || Peek() == "\r")
        Advance();
}

void Scanner::DropWS()
{
    while (tokens.size() > 0 && tokens.back().GetType() == WHITESPACE)
        tokens.pop_back();
}

void Scanner::ScanCommandOrSpecial()
{
    if (isalnum(Peek()) {
        /// Commands are alphanum
        while (isalnum(Peek()) || Peek() == '_')
            Advance();
        string lexeme = source.substr(start, current - start);
        if (lexeme == "begin")
            AddToken(BEGIN_ENV);
        else if (lexeme == "end")
            AddToken(END_ENV);
        else
            AddToken(COMMAND, lexeme);
        if (!Match('\\'))
            EatWS();
    }
    else if (Match('\''))
    {
        if (!isalnum(Peek()))
        {
            Error("Illegal single quote escape.");
            Advance();
        }
        else
            AddToken(QUOT, string(Advance()));
    }
    else if (Match('"'))
    {
        if (!isalnum(Peek()))
        {
            Error("Illegal doubgle quote escape.");
            Advance();
        }
        else
            AddToken(DUQUOT, string(Advance()));
    }
    else if (Match('`'))
    {
        if (!isalnum(Peek()))
        {
            Error("Illegal tick escape.");
            Advance();
        }
        else
            AddToken(TICK, string(Advance()));
    }
    else if (Match('^'))
    {
        if (!isalnum(Peek()))
        {
            Error("Illegal hat escape.");
            Advance();
        }
        else
            AddToken(HAT, string(Advance()));
    }
    else if (Match('~'))
    {
        if (!isalnum(Peek()))
        {
            Error("Illegal tilde escape.");
            Advance();
        }
        else
            AddToken(TILT, string(Advance()));
    }
    else if (Match('.'))
    {
        if (!isalnum(Peek()))
        {
            Error("Illegal dot escape.");
            Advance();
        }
        else
            AddToken(DOT, string(Advance()));
    }
    else if (Match('-'))
    {
        if (!isalnum(Peek()))
        {
            Error("Illegal dash escape.");
            Advance();
        }
        else
            AddToken(DASH, string(Advance()));
    }
    else if (Match('%'))
    {
        AddToken(WORD, "%");
    }
    else if (Match('{'))
    {
        AddToken(WORD, "{");
    }
    else if (Match('}'))
    {
        AddToken(WORD, "}");
    }
    else if (Match('&'))
    {
        AddToken(WORD, "&");
    }
    else if (Match('\\'))
    {
        AddToken(NEWLINE);
    }
    else if (Match('<'))
    {
        AddToken(LESS);
    }
    else if (Match('>'))
    {
        AddToken(GREATER);
    }
}

bool is_valid_char(char c)
{
    if (c < 32)
        return false;
    if (c > 126)
        return false;
    return (c != '<' && c != '>' && c != '[' && c != ']' && c != ' ' && c != '\t' && 
            c != '{' && c != '}' && c != '%' && c != '\\' && c != '&');
}

void Scanner::ScanToken()
{
    char c = Advance();
    switch (c)
    {
    case '[':
        AddToken(LEFT_BRACKET);
        EatWS();
        break;
    case ']':
        DropWS();
        AddToken(RIGHT_BRACKET);
        break;
    case '{':
        AddToken(LEFT_BRACE);
        EatWS();
        break;
    case '}':
        DropWS();
        AddToken(RIGHT_BRACE);
        break;
    case '&':
        AddToken(AMPERSAND);
        break;
    case '\\':
        ScanCommandOrSpecial();
        break;
    case '%':
        while (!IsAtEnd() && Peek() != '\n')
            Advance();
        break;
    case ' ':
    case '\t':
        string literal(c);

        while (Peek() == ' ' || Peek() == '\t')
            literal += Advance();

        AddToken(WHITESPACE, literal);
    case '\r':
        break;
    case '\n':
        line++;
        AddToken(LINE, c);
        break;
    default:
        string literal(c);
        while (!IsAtEnd() && is_valid_char(Peek()))
        {
            literal.push_back(Peek());
            Advance();
        }
        AddToken(WORD, literal);
    }
}

Scanner::Scanner(string source)
    : source(source) {}

vector<Token> Scanner::ScanTokens()
{
    while (!IsAtEnd())
    {
        start = current;
        ScanToken();
    }
    tokens.push_back(Token(EOTF, "", line));
    return tokens;
}