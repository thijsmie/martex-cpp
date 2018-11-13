#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

class Interpreter;

#include "expr.hpp"
#include "value.hpp"

class Interpreter : public ExprVisitor
{
  Value value; // used to return things

  std::shared_ptr<Environment> environment;
  std::shared_ptr<Environment> globals;
  Implementation implementation;

  vector<Value> ExecuteBlock(std::vector<std::shared_ptr<const Expr>>, std::shared_ptr<Environment>);

  RuntimeError Error(Token, std::string);

  void VisitBlockExpr(std::shared_ptr<const BlockExpr>);
  void VisitLiteralExpr(std::shared_ptr<const LiteralExpr>);
  void VisitActionableExpr(std::shared_ptr<const ActionableExpr>);
  void VisitCommandExpr(std::shared_ptr<const CommandExpr>);
  void VisitEnvironmentExpr(std::shared_ptr<const EnvironmentExpr>);

public:
  Interpreter(Implementation);
  Value Evaluate(std::shared_ptr<const Expr>);
};

#endif // INTERPRETER_H