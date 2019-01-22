#pragma once

#define MAX_STACK_DEPTH 2000

#include <iostream>
#include <memory>
#include <map>
#include <vector>

class Interpreter;

#include "expr.hpp"
#include "value.hpp"
#include "environment.hpp"
#include "error_reporter.hpp"
#include "runtime_error.hpp"
#include "core/implementation/implementation.hpp"

class Interpreter : public ExprVisitor
{
    std::shared_ptr<Environment> environment;
    std::shared_ptr<Environment> globals;
    std::shared_ptr<Implementation> implementation;
    ErrorReporter &error_reporter;

    int StackDepth = 0;

    std::map<std::string, std::shared_ptr<const Expr>> runnables;

    Value ExecuteBlock(std::vector<std::shared_ptr<const Expr>>);

    RuntimeError Error(Token, std::string);

    Value VisitBlockExpr(std::shared_ptr<const BlockExpr>);
    Value VisitLiteralExpr(std::shared_ptr<const LiteralExpr>);
    Value VisitActionableExpr(std::shared_ptr<const ActionableExpr>);
    Value VisitCommandExpr(std::shared_ptr<const CommandExpr>);
    Value VisitEnvironmentExpr(std::shared_ptr<const EnvironmentExpr>);
    Value VisitNewCommandExpr(std::shared_ptr<const NewCommandExpr>);
    Value VisitVarExpr(std::shared_ptr<const VarExpr>);

    void IncrStack();
    void DecrStack();

  public:
    Interpreter(std::shared_ptr<Environment>, std::shared_ptr<Implementation>, ErrorReporter &error_reporter);
    Value Evaluate(std::shared_ptr<const Expr>);
};
