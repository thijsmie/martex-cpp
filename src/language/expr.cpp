#include "expr.hpp"

using std::nullptr_t;
using std::shared_ptr;
using std::string;
using std::vector;

BlockExpr::BlockExpr(vector<shared_ptr<const Expr>> expressions) : expressions(expressions){};

Value BlockExpr::Accept(ExprVisitor *visitor) const
{
    return visitor->VisitBlockExpr(shared_from_this());
}

void BlockExpr::Print(std::ostream &o) const
{
    o << "[block " << expressions.size() << "]\n";
    for (auto e : expressions)
        e->Print(o);
    o << "[endblock]\n";
}

LiteralExpr::LiteralExpr(Token value) : value(value){};

Value LiteralExpr::Accept(ExprVisitor *visitor) const
{
    return visitor->VisitLiteralExpr(shared_from_this());
}

void LiteralExpr::Print(std::ostream &o) const
{
    o << "[literal=" << value.GetLexeme() << "] ";
}

ActionableExpr::ActionableExpr(Token value) : value(value){};

Value ActionableExpr::Accept(ExprVisitor *visitor) const
{
    return visitor->VisitActionableExpr(shared_from_this());
}

void ActionableExpr::Print(std::ostream &o) const
{
    o << "[literal=" << value.GetLexeme() << "] ";
}

CommandExpr::CommandExpr(Token command, std::vector<std::shared_ptr<const Expr>> arguments) : command(command), arguments(arguments){};

Value CommandExpr::Accept(ExprVisitor *visitor) const
{
    return visitor->VisitCommandExpr(shared_from_this());
}

void CommandExpr::Print(std::ostream &o) const
{
    o << "[command=" << command.GetLexeme() << "] ";
    for (auto e : arguments)
        e->Print(o);
    o << "[endcommand]\n";
}

EnvironmentExpr::EnvironmentExpr(Token begin, Token end, std::shared_ptr<const Expr> bracket_argument,
                                 std::shared_ptr<const Expr> block) : begin(begin), end(end),
                                                                      bracket_argument(bracket_argument),
                                                                      block(block){};

Value EnvironmentExpr::Accept(ExprVisitor *visitor) const
{
    return visitor->VisitEnvironmentExpr(shared_from_this());
}

void EnvironmentExpr::Print(std::ostream &o) const
{
    o << "[env=" << begin.GetLexeme() << "] ";
    block->Print(o);
    o << "[endevn]\n";
}

NewCommandExpr::NewCommandExpr(Token newcommand, Token name, int numargs,
                               std::shared_ptr<const Expr> content) : newcommand(newcommand), name(name),
                                                                      numargs(numargs),
                                                                      content(content){};

Value NewCommandExpr::Accept(ExprVisitor *visitor) const
{
    return visitor->VisitNewCommandExpr(shared_from_this());
}

void NewCommandExpr::Print(std::ostream &o) const
{
}

VarExpr::VarExpr(Token variable) : variable(variable) {}

Value VarExpr::Accept(ExprVisitor *visitor) const
{
    return visitor->VisitVarExpr(shared_from_this());
}

void VarExpr::Print(std::ostream &o) const {}