#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

class Interpreter;

#include "expr.hpp"
#include "value.hpp"
#include "environment.hpp"
#include "implementation/implementation.hpp"

class Interpreter : public ExprVisitor
{
    ErrorReporter &error_reporter;
    Value value; // used to return things

    std::shared_ptr<Environment> environment;
    std::shared_ptr<Environment> globals;
    std::shared_ptr<Implementation> implementation;

    Value ExecuteBlock(std::vector<std::shared_ptr<const Expr>>);

    void Error(Token, std::string);

    void VisitBlockExpr(std::shared_ptr<const BlockExpr>);
    void VisitLiteralExpr(std::shared_ptr<const LiteralExpr>);
    void VisitActionableExpr(std::shared_ptr<const ActionableExpr>);
    void VisitCommandExpr(std::shared_ptr<const CommandExpr>);
    void VisitEnvironmentExpr(std::shared_ptr<const EnvironmentExpr>);

  public:
    Interpreter(std::shared_ptr<Implementation>, ErrorReporter &error_reporter);
    Value Evaluate(std::shared_ptr<const Expr>);
};
