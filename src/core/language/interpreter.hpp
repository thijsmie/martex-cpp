#pragma once

#define MAX_STACK_DEPTH 500

#include <iostream>
#include <memory>
#include <map>
#include <vector>

class Interpreter;

#include "expr.hpp"
#include "value.hpp"
#include "environment.hpp"
#include "module.hpp"
#include "error_reporter.hpp"
#include "runtime_error.hpp"
#include "implementation.hpp"

class Interpreter : public ExprVisitor
{
  private:
    std::vector<std::shared_ptr<Module>> modules;
    std::shared_ptr<Environment> environment;
    std::shared_ptr<Environment> globals;
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

    std::shared_ptr<Environment> CreateEnvironment(Token, std::shared_ptr<Environment>);

  public:
    Interpreter(std::shared_ptr<Environment>, std::vector<std::shared_ptr<Module>>, ErrorReporter &error_reporter);
    Value Evaluate(std::shared_ptr<const Expr>);
};
