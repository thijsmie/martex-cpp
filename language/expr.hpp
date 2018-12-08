#ifndef EXPR_H_
#define EXPR_H_

#include "token.hpp"
#include <vector>
#include <memory>
#include <iostream>

template <typename T>
using Visitable = std::enable_shared_from_this<T>;

class ExprVisitor;

class Expr
{
  public:
    virtual void Accept(ExprVisitor *) const = 0;
    virtual void Print(std::ostream&o) const = 0;
};

class BlockExpr : public Expr, public Visitable<BlockExpr>
{
  public:
    BlockExpr(std::vector<std::shared_ptr<const Expr>>);
    /*virtual*/ void Accept(ExprVisitor *) const;
    void Print(std::ostream&o) const;

    std::vector<std::shared_ptr<const Expr>> expressions;
};

class LiteralExpr : public Expr, public Visitable<LiteralExpr>
{
  public:
    LiteralExpr(Token);
    /*virtual*/ void Accept(ExprVisitor *) const;
    void Print(std::ostream&o) const;

    Token value;
};

class ActionableExpr : public Expr, public Visitable<ActionableExpr>
{
  public:
    ActionableExpr(Token);
    /*virtual*/ void Accept(ExprVisitor *) const;
    void Print(std::ostream&o) const;

    Token value;
};

class CommandExpr : public Expr, public Visitable<CommandExpr>
{
  public:
    CommandExpr(Token, std::vector<std::shared_ptr<const Expr>>);
    /*virtual*/ void Accept(ExprVisitor *) const;
    void Print(std::ostream&o) const;

    Token command;
    std::vector<std::shared_ptr<const Expr>> arguments;
};

class EnvironmentExpr : public Expr, public Visitable<EnvironmentExpr>
{
  public:
    EnvironmentExpr(Token, Token, std::shared_ptr<const Expr>, std::shared_ptr<const Expr>);
    /*virtual*/ void Accept(ExprVisitor *) const;
    void Print(std::ostream&o) const;

    Token begin;
    Token end;
    std::shared_ptr<const Expr> bracket_argument;
    std::shared_ptr<const Expr> block;
};

class ExprVisitor
{
  public:
    virtual void VisitBlockExpr(std::shared_ptr<const BlockExpr>) = 0;
    virtual void VisitLiteralExpr(std::shared_ptr<const LiteralExpr>) = 0;
    virtual void VisitActionableExpr(std::shared_ptr<const ActionableExpr>) = 0;
    virtual void VisitCommandExpr(std::shared_ptr<const CommandExpr>) = 0;
    virtual void VisitEnvironmentExpr(std::shared_ptr<const EnvironmentExpr>) = 0;
};

#endif // EXPR_H_