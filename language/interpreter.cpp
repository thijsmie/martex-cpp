#include "interpreter.hpp"

using std::dynamic_pointer_cast;
using std::make_shared;
using std::shared_ptr;
using std::static_pointer_cast;
using std::string;
using std::vector;

Interpreter::Interpreter(shared_ptr<Implementation> i) : implementation(i)
{
    environment = implementation->Global();
    globals = environment;
}

Value Interpreter::Evaluate(shared_ptr<const Expr> expr)
{
    expr->Accept(this);
    return value;
}

Value Interpreter::ExecuteBlock(vector<shared_ptr<const Expr>> expressions)
{
    vector<Value> result;
    for (shared_ptr<const Expr> expr : expressions)
    {
        result.push_back(Evaluate(expr));
    }
    return Value(result);
}

RuntimeError Interpreter::Error(Token token, string message)
{
    return RuntimeError(token, message);
}

void Interpreter::VisitBlockExpr(shared_ptr<const BlockExpr> block)
{
    // return
    value = ExecuteBlock(block->expressions);
}

void Interpreter::VisitLiteralExpr(shared_ptr<const LiteralExpr> literal)
{
    switch (literal->value.GetType())
    {
    case TokenType::WORD:
    case TokenType::LINE:
    case TokenType::WHITESPACE:
        value = Value(t_string, literal->value.ToString());
        break;
    case TokenType::NEWLINE:
        value = Value(t_break, implementation->LineBreak());
        break;
    case TokenType::AMPERSAND:
        value = Value(t_ampersand, implementation->Ampersand());
        break;
    }
}

void Interpreter::VisitActionableExpr(shared_ptr<const ActionableExpr> actionable)
{
    // return
    value = Value(t_string, implementation->Escaped(actionable->value.GetType(), actionable->value.ToString()[0]));
}

void Interpreter::VisitCommandExpr(shared_ptr<const CommandExpr> command)
{
    vector<Value> arguments;
    arguments.reserve(command->arguments.size());

    for (auto argument : command->arguments)
        arguments.push_back(Evaluate(argument));

    // return
    value = environment->RunCommand(command->command, arguments);
}

void Interpreter::VisitEnvironmentExpr(shared_ptr<const EnvironmentExpr> env)
{
    shared_ptr<Environment> current = environment;
    try
    {
        environment = implementation->Create(env.get()->environment, environment);
        Value bracket_argument;

        if (env.get()->bracket_argument != nullptr)
            bracket_argument = Evaluate(env.get()->bracket_argument);

        environment->StartEnvironment(bracket_argument);

        Value v = ExecuteBlock(env.get()->block);

        environment->EndEnvironment(v);
    }
    catch (RuntimeError e)
    {
        environment = current;
        throw e;
    }
    environment = current;
};