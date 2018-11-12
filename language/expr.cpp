#include "expr.hpp"

using std::shared_ptr;
using std::string;
using std::nullptr_t;
using std::vector;

BlockExpr::BlockExpr(shared_ptr<vector<const Expr>> expressions) : expressions(expressions) {};

void BlockExpr::Accept(ExprVisitor* visitor) const {
  visitor->VisitBlockExpr(shared_from_this());
}

LiteralExpr::LiteralExpr(Token value) : value(value) {};

void LiteralExpr::Accept(ExprVisitor* visitor) const {
  visitor->VisitLiteralExpr(shared_from_this());
}

ActionableExpr::ActionableExpr(Token value) : value(value) {};

void ActionableExpr::Accept(ExprVisitor* visitor) const {
  visitor->VisitActionableExpr(shared_from_this());
}

