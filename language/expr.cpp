#include "expr.hpp"

using std::nullptr_t;
using std::shared_ptr;
using std::string;
using std::vector;

BlockExpr::BlockExpr(vector<shared_ptr<const Expr>> expressions) : expressions(expressions){};

void BlockExpr::Accept(ExprVisitor *visitor) const
{
    visitor->VisitBlockExpr(shared_from_this());
}

LiteralExpr::LiteralExpr(Token value) : value(value){};

void LiteralExpr::Accept(ExprVisitor *visitor) const
{
    visitor->VisitLiteralExpr(shared_from_this());
}

ActionableExpr::ActionableExpr(Token value) : value(value){};

void ActionableExpr::Accept(ExprVisitor *visitor) const
{
    visitor->VisitActionableExpr(shared_from_this());
}

CommandExpr::CommandExpr(Token command, std::vector<std::shared_ptr<const Expr>> arguments) : command(command), arguments(arguments){};

void CommandExpr::Accept(ExprVisitor *visitor) const
{
    visitor->VisitCommandExpr(shared_from_this());
}

EnvironmentExpr::EnvironmentExpr(Token environment, std::shared_ptr<const Expr> bracket_argument,
                                 std::shared_ptr<const Expr> block) : environment(environment),
                                                                      bracket_argument(bracket_argument),
                                                                      block(block){};

void EnvironmentExpr::Accept(ExprVisitor *visitor) const
{
    visitor->VisitEnvironmentExpr(shared_from_this());
}
