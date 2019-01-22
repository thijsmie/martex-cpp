#pragma once

#include "token.hpp"
#include "value.hpp"
#include <vector>
#include <memory>
#include <iostream>

template <typename T>
using Visitable = std::enable_shared_from_this<T>;

class ExprVisitor;

class Expr
{
  public:
    virtual Value Accept(ExprVisitor *) const = 0;
    virtual void Print(std::ostream&o) const = 0;
};

class BlockExpr : public Expr, public Visitable<BlockExpr>
{
  public:
    BlockExpr(std::vector<std::shared_ptr<const Expr>>);
    /*virtual*/ Value Accept(ExprVisitor *) const;
    void Print(std::ostream&o) const;

    std::vector<std::shared_ptr<const Expr>> expressions;
};

class LiteralExpr : public Expr, public Visitable<LiteralExpr>
{
  public:
    LiteralExpr(Token);
    /*virtual*/ Value Accept(ExprVisitor *) const;
    void Print(std::ostream&o) const;

    Token value;
};

class ActionableExpr : public Expr, public Visitable<ActionableExpr>
{
  public:
    ActionableExpr(Token);
    /*virtual*/ Value Accept(ExprVisitor *) const;
    void Print(std::ostream&o) const;

    Token value;
};

class CommandExpr : public Expr, public Visitable<CommandExpr>
{
  public:
    CommandExpr(Token, std::vector<std::shared_ptr<const Expr>>);
    /*virtual*/ Value Accept(ExprVisitor *) const;
    void Print(std::ostream&o) const;

    Token command;
    std::vector<std::shared_ptr<const Expr>> arguments;
};

class EnvironmentExpr : public Expr, public Visitable<EnvironmentExpr>
{
  public:
    EnvironmentExpr(Token, Token, std::shared_ptr<const Expr>, std::shared_ptr<const Expr>);
    /*virtual*/ Value Accept(ExprVisitor *) const;
    void Print(std::ostream&o) const;

    Token begin;
    Token end;
    std::shared_ptr<const Expr> bracket_argument;
    std::shared_ptr<const Expr> block;
};

class NewCommandExpr : public Expr, public Visitable<NewCommandExpr>
{
  public:
    NewCommandExpr(Token newcommand, Token name, int newargs, std::shared_ptr<const Expr> content);
    /*virtual*/ Value Accept(ExprVisitor *) const;
    void Print(std::ostream&o) const;

    Token newcommand;
    Token name;
    int numargs;
    std::shared_ptr<const Expr> content;
};

class VarExpr : public Expr, public Visitable<VarExpr>
{
  public:
    VarExpr(Token variable);
    Value Accept(ExprVisitor *) const;
    void Print(std::ostream&o) const;

    Token variable;
};

class ExprVisitor
{
  public:
    virtual Value VisitBlockExpr(std::shared_ptr<const BlockExpr>) = 0;
    virtual Value VisitLiteralExpr(std::shared_ptr<const LiteralExpr>) = 0;
    virtual Value VisitActionableExpr(std::shared_ptr<const ActionableExpr>) = 0;
    virtual Value VisitCommandExpr(std::shared_ptr<const CommandExpr>) = 0;
    virtual Value VisitEnvironmentExpr(std::shared_ptr<const EnvironmentExpr>) = 0;
    virtual Value VisitNewCommandExpr(std::shared_ptr<const NewCommandExpr>) = 0;
    virtual Value VisitVarExpr(std::shared_ptr<const VarExpr>) = 0;
};
