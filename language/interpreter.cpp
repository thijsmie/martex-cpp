#include "interpreter.hpp"

using std::dynamic_pointer_cast;
using std::make_shared;
using std::shared_ptr;
using std::static_pointer_cast;
using std::string;
using std::vector;

Interpreter::Interpreter(shared_ptr<Implementation> i, ErrorReporter &reporter) : implementation(i), error_reporter(reporter)
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
        try
        {
            result.push_back(Evaluate(expr));
        }
        catch (RuntimeError e)
        {
            // test
            error_reporter.Error(e.token, e.message);
            error_reporter.Log("Ignoring and moving on.");
        }
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
        value = Value(t_string, literal->value.GetLexeme());
        break;
    case TokenType::NEWLINE:
        value = Value(t_break, implementation->LineBreak());
        break;
    case TokenType::AMPERSAND:
        value = Value(t_ampersand, implementation->Ampersand());
        break;
    default:
        value = Value();
    }
}

void Interpreter::VisitActionableExpr(shared_ptr<const ActionableExpr> actionable)
{
    // return
    value = Value(t_string, implementation->Escaped(actionable->value.GetType(), actionable->value.GetLexeme().at(0)));
}

void Interpreter::VisitCommandExpr(shared_ptr<const CommandExpr> command)
{
    vector<Value> arguments;
    arguments.reserve(command->arguments.size());

    for (auto argument : command->arguments)
        arguments.push_back(Evaluate(argument));

    // return
    value = environment->RunCommand(environment, command->command, arguments);
}

void Interpreter::VisitEnvironmentExpr(shared_ptr<const EnvironmentExpr> env)
{
    shared_ptr<Environment> current = environment;
    try
    {
        environment = implementation->Create(env->begin, environment);
        Value bracket_argument;

        if (env->bracket_argument != nullptr)
            bracket_argument = Evaluate(env->bracket_argument);

        environment->StartEnvironment(env->begin, bracket_argument);

        Value v = Evaluate(env->block);

        value = environment->EndEnvironment(env->end, v);
    }
    catch (RuntimeError e)
    {
        environment = current;
        throw e;
    }
    environment = current;
};