#pragma once

#include <string>
#include <vector>
#include <exception>
#include <memory>

#include "token.hpp"
#include "expr.hpp"
#include "error_reporter.hpp"

class ParseError : public std::exception
{
    const char *what() const throw()
    {
        return "Parser error";
    }
};

class Parser
{
  public:
    Parser(std::vector<Token> &tokens, ErrorReporter &error_reporter);

    std::shared_ptr<const Expr> Parse();

  private:
    std::vector<Token> tokens;
    ErrorReporter &error_reporter;

    int current = 0; // current token

    bool IsAtEnd(const int &offset = 1);
    Token Peek();
    Token Previous();
    Token Advance();
    bool Check(const TokenType &tokenType);
    bool CheckAhead(const TokenType &tokenTypez, const int &offset = 1);
    bool Match(const std::vector<TokenType> &tokenTypes);
    void Ignore(const std::vector<TokenType> &tokenTypes);
    bool CheckIgnore(const std::vector<TokenType> &ignoreTypes, const std::vector<TokenType> &matchTypes);

    Token Consume(TokenType type, std::string message);
    ParseError Error(Token token, std::string message);

    std::vector<std::shared_ptr<const Expr>> Block();

    std::shared_ptr<const Expr> BlockExpression();
    std::shared_ptr<const Expr> Expression();
    std::shared_ptr<const Expr> Literal();
    std::shared_ptr<const Expr> Actionable();
    std::shared_ptr<const Expr> Command();
    std::shared_ptr<const Expr> Environment();
    std::shared_ptr<const Expr> Bracketed();
    std::shared_ptr<const Expr> Braced();
};
