#include "parser.hpp"
#include "interpreter.hpp"
#include "core/util/string_util.hpp"

using std::dynamic_pointer_cast;
using std::static_pointer_cast;

using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;

bool Parser::IsAtEnd(const int &offset)
{
    return ((int)tokens.size()) - 1 < current + offset;
}

Token Parser::Peek()
{
    return tokens[current];
}

Token Parser::Previous()
{
    return tokens[current - 1];
}

Token Parser::Advance()
{
    if (!IsAtEnd())
        current++;
    return Previous();
}

bool Parser::Check(const TokenType &tokenType)
{
    return !IsAtEnd() && Peek().GetType() == tokenType;
}

bool Parser::CheckAhead(const TokenType &tokenType, const int &offset)
{
    return !IsAtEnd(offset) && tokens[current + offset].GetType() == tokenType;
}

bool Parser::Match(const vector<TokenType> &tokenTypes)
{
    for (const TokenType &tokenType : tokenTypes)
    {
        if (Check(tokenType))
        {
            Advance();
            return true;
        }
    }
    return false;
}

void Parser::Ignore(const vector<TokenType> &tokenTypes)
{
    while (true)
    {
        for (const TokenType &tokenType : tokenTypes)
        {
            if (Check(tokenType))
            {
                Advance();
                continue;
            }
        }
        break;
    }
}

bool Parser::CheckIgnore(const vector<TokenType> &ignoreTypes, const vector<TokenType> &matchTypes)
{
    int offset = 0;
    while (true)
    {
        for (const TokenType &ignoreType : ignoreTypes)
        {
            if (CheckAhead(ignoreType, offset))
            {
                ++offset;
                continue;
            }
        }
        for (const TokenType &matchType : matchTypes)
        {
            if (CheckAhead(matchType, offset))
            {
                return true;
            }
        }
        return false;
    }
}

Token Parser::Consume(TokenType type, string message)
{
    if (Check(type))
        return Advance();
    throw Error(Peek(), message);
}

ParseError Parser::Error(Token token, string message)
{
    error_reporter.Error(token, message);
    return ParseError();
}

vector<shared_ptr<const Expr>> Parser::Block()
{
    vector<shared_ptr<const Expr>> expressions;

    while ((!Check(RIGHT_BRACE)) && (!Check(RIGHT_BRACKET)) && (!Check(END_ENV)) && (!IsAtEnd()))
    {
        try
        {
            expressions.push_back(Expression());
        }
        catch (ParseError e)
        {
            error_reporter.Log(Peek(), "Trying to recover from here.");
        }
    }

    return expressions;
}

shared_ptr<const Expr> Parser::BlockExpression()
{
    return make_shared<BlockExpr>(Block());
}

std::shared_ptr<const Expr> Parser::Expression()
{
    if (Match({WORD, WHITESPACE, LINE, AMPERSAND, NEWLINE}))
    {
        return Literal();
    }
    else if (Match({QUOT, DUQUOT, TICK, HAT, TILT, DOT, DASH, ESCAPE}))
    {
        return Actionable();
    }
    else if (Check(COMMAND))
    {
        return Command();
    }
    else if (Check(NEWCOMMAND))
    {
        return NewCommand();
    }
    else if (Check(VAR))
    {
        return Variable();
    }
    else if (Check(BEGIN_ENV))
    {
        return Environment();
    }
    throw Error(Advance(), "Invalid syntax detected, attempting to recover...");
}

std::shared_ptr<const Expr> Parser::Literal()
{
    Token literal = Previous();
    return make_shared<LiteralExpr>(literal);
}

std::shared_ptr<const Expr> Parser::Actionable()
{
    Token actionable = Previous();
    return make_shared<ActionableExpr>(actionable);
}

std::shared_ptr<const Expr> Parser::Command()
{
    Token command = Advance();

    vector<shared_ptr<const Expr>> arguments;

    while (CheckIgnore({WHITESPACE}, {LEFT_BRACE}))
    {
        Ignore({WHITESPACE});
        arguments.push_back(Braced());
    }

    return make_shared<CommandExpr>(command, arguments);
}

std::shared_ptr<const Expr> Parser::Environment()
{
    Token begin = Advance();

    shared_ptr<const Expr> bracket_arg = nullptr;

    // We dont use Braced here since dynamically named envs are not allowed
    Ignore({WHITESPACE});
    Consume(LEFT_BRACE, "No environment specified with begin.");
    Ignore({WHITESPACE});
    Token b_envname = Consume(WORD, "No environment specified with begin.");
    Ignore({WHITESPACE});
    Consume(RIGHT_BRACE, "Missing close brace after environment name.");

    if (CheckIgnore({WHITESPACE}, {LEFT_BRACKET}))
    {
        Ignore({WHITESPACE});
        bracket_arg = Bracketed();
    }

    // Environment contents
    shared_ptr<const Expr> block = BlockExpression();

    Consume(END_ENV, "No matching end tag for environment " + b_envname.GetLexeme() + "(" + std::to_string(begin.GetLine()) + ")");
    Consume(LEFT_BRACE, "No environment specified with end.");
    Token e_envname = Consume(WORD, "No environment specified with end.");
    Consume(RIGHT_BRACE, "Missing close brace after environment name.");

    if (b_envname.GetLexeme() != e_envname.GetLexeme())
        throw Error(e_envname, "End tag for " + e_envname.GetLexeme() + " but expected " + b_envname.GetLexeme() + "(" + std::to_string(begin.GetLine()) + ")");

    return make_shared<EnvironmentExpr>(b_envname, e_envname, bracket_arg, block);
}

std::shared_ptr<const Expr> Parser::Bracketed()
{
    Consume(LEFT_BRACKET, "Expected [");
    shared_ptr<const Expr> argument = BlockExpression();
    Consume(RIGHT_BRACKET, "Expected ]");
    return argument;
}

std::shared_ptr<const Expr> Parser::Braced()
{
    Consume(LEFT_BRACE, "Expected }");
    shared_ptr<const Expr> argument = BlockExpression();
    Consume(RIGHT_BRACE, "Expected }");
    return argument;
}

std::shared_ptr<const Expr> Parser::NewCommand()
{
    Token newcommand = Advance();

    bool eatrs = false;
    if (CheckIgnore({WHITESPACE}, {LEFT_BRACE}))
    {
        Ignore({WHITESPACE});
        Advance();
        eatrs = true;
    }

    Token command = Consume(COMMAND, "Expected a command to define");

    Ignore({WHITESPACE});
    if (eatrs) {
        Consume(RIGHT_BRACE, "Expected }");
        Ignore({WHITESPACE});
    }

    int numargs = 0;
    if (Check(LEFT_BRACKET))
    {
        Token lb = Advance();
        if (!CheckIgnore({WHITESPACE, WORD}, {RIGHT_BRACKET}))
            throw Error(lb, "Expected a number and then ]");

        std::string numargs_ = "";

        while(!Check(RIGHT_BRACKET))
        {
            Token w = Advance();
            numargs_ += w.GetLexeme();
        }
        Token rb = Advance();

        util::trim(numargs_);

        if(!util::dgonly(numargs_))
            throw Error(rb, "Expected a number and then ]");

        numargs = std::stoi(numargs_);
        Ignore({WHITESPACE});
    }

    Consume(LEFT_BRACE, "Expected {");
    shared_ptr<const Expr> block = BlockExpression();
    Consume(RIGHT_BRACE, "Expected }");

    return make_shared<NewCommandExpr>(newcommand, command, numargs, block);
}

shared_ptr<const Expr> Parser::Variable()
{
    Token var = Advance();
    return make_shared<VarExpr>(var);
}

shared_ptr<const Expr> Parser::Parse()
{
    shared_ptr<const Expr> content = BlockExpression();
    if (!IsAtEnd())
        Error(Advance(), "Expected end of document.");
    return content;
}

Parser::Parser(vector<Token> &tokens, ErrorReporter &error_reporter) : tokens(tokens), error_reporter(error_reporter) {}
