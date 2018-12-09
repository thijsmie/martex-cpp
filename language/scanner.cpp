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
    while (Peek() == ' ' || Peek() == '\t' || Peek() == '\r')
        Advance();
}

void Scanner::DropWS()
{
    while (tokens.size() > 0 && tokens.back().GetType() == WHITESPACE)
        tokens.pop_back();
}

void Scanner::ScanCommandOrSpecial()
{
    if (isalnum(Peek()))
    {
        /// Commands are alphanum
        while (isalnum(Peek()) || Peek() == '_')
            Advance();
        string lexeme = source.substr(start+1, current - start - 1);
        if (lexeme == "begin")
            AddToken(BEGIN_ENV);
        else if (lexeme == "end")
            AddToken(END_ENV);
        else /// implement other keywords like 'if', 'else', 'foreach' etc here
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
            AddToken(QUOT, string(1, Advance()));
    }
    else if (Match('"'))
    {
        if (!isalnum(Peek()))
        {
            Error("Illegal doubgle quote escape.");
            Advance();
        }
        else
            AddToken(DUQUOT, string(1, Advance()));
    }
    else if (Match('`'))
    {
        if (!isalnum(Peek()))
        {
            Error("Illegal tick escape.");
            Advance();
        }
        else
            AddToken(TICK, string(1, Advance()));
    }
    else if (Match('^'))
    {
        if (!isalnum(Peek()))
        {
            Error("Illegal hat escape.");
            Advance();
        }
        else
            AddToken(HAT, string(1, Advance()));
    }
    else if (Match('~'))
    {
        if (!isalnum(Peek()))
        {
            Error("Illegal tilde escape.");
            Advance();
        }
        else
            AddToken(TILT, string(1, Advance()));
    }
    else if (Match('.'))
    {
        if (!isalnum(Peek()))
        {
            Error("Illegal dot escape.");
            Advance();
        }
        else
            AddToken(DOT, string(1, Advance()));
    }
    else if (Match('-'))
    {
        if (!isalnum(Peek()))
        {
            Error("Illegal dash escape.");
            Advance();
        }
        else
            AddToken(DASH, string(1, Advance()));
    }
    else if (Match('%'))
    {
        AddToken(ESCAPE, "%");
    }
    else if (Match('{'))
    {
        AddToken(ESCAPE, "{");
    }
    else if (Match('}'))
    {
        AddToken(ESCAPE, "}");
    }
    else if (Match('&'))
    {
        AddToken(ESCAPE, "&");
    }
    else if (Match('\\'))
    {
        AddToken(NEWLINE);
    }
    else if (Match('<'))
    {
        AddToken(ESCAPE, "<");
    }
    else if (Match('>'))
    {
        AddToken(ESCAPE, ">");
    }
}

bool is_valid_char(char c)
{
    if (c < 33)
        return false;
    if (c > 126)
        return false;

    /// The range defined above includes chars:
    /// !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ
    /// [\]^_`abcdefghijklmnopqrstuvwxyz{|}~

    /// In future: filter $ too
    return (c != '<' && c != '>' && c != '[' && c != ']' &&
            c != '{' && c != '}' && c != '%' && c != '\\' && c != '&');
}

void Scanner::ScanToken()
{
    char c = Advance();
    string literal(1, c);
    switch (c)
    {
    case '[':
        AddToken(LEFT_BRACKET);
        break;
    case ']':
        AddToken(RIGHT_BRACKET);
        break;
    case '{':
        AddToken(LEFT_BRACE);
        break;
    case '}':
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
        while (Peek() == ' ' || Peek() == '\t')
            literal += Advance();

        AddToken(WHITESPACE, literal);
        break;
    case '\r':
        /// Windows line endings? We don't give a damn, skip!
        break;
    case '\n':
        line++;
        AddToken(LINE, literal);
        break;
    default:
        while (!IsAtEnd() && is_valid_char(Peek()))
        {
            literal.push_back(Peek());
            Advance();
        }
        AddToken(WORD, literal);
        break;
    }
}

Scanner::Scanner(string source, ErrorReporter& error_reporter)
    : source(source), error_reporter(error_reporter) {}

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

void Scanner::Error(string message)
{
    error_reporter.Report(line, std::string(1, Peek()), message);
}