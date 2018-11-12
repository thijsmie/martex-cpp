#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

class Interpreter;

#include "expr.hpp"

class Interpreter : public ExprVisitor
{
  std::shared_ptr<Environment> environment;
  std::shared_ptr<Environment> globals;
  std::unordered_map<std::shared_ptr<const Expr>, int> locals;

  Value Evaluate(std::shared_ptr<const Expr>);
  Value LookUpVariable(Token, std::shared_ptr<const Expr>);

  RuntimeError Error(Token, std::string);

  void VisitBlockExpr(std::shared_ptr<const BlockExpr>);
  void VisitLiteralExpr(std::shared_ptr<const LiteralExpr>);
  void VisitActionableExpr(std::shared_ptr<const ActionableExpr>);
  void VisitCommandExpr(std::shared_ptr<const CommandExpr>);
  void VisitEnvironmentExpr(std::shared_ptr<const EnvironmentExpr>);

public:
  Interpreter();
  void Interpret(const std::vector<std::shared_ptr<const Expr>> &);
  void Resolve(std::shared_ptr<const Expr>, int);
  void Execute(std::shared_ptr<const Expr>);
  void ExecuteBlock(std::vector<std::shared_ptr<const Expr>>,
                    std::shared_ptr<Environment>);
};

#endif // INTERPRETER_H