#include "martex.hpp"

#include "php/phpmodule.hpp"
#include "language/scanner.hpp"
#include "language/parser.hpp"
#include "cpp/stdlib.hpp"

using std::shared_ptr;
using std::string;
using std::vector;

MarTeX::MarTeX() : has_error(false), last_result(""), error_log("") {}

void MarTeX::RegisterModule(Php::Parameters &params)
{
    string module_name = params[0];
    modules.push_back(module_name);
}

void MarTeX::Parse(Php::Parameters &params)
{
    // New parse, clear last result
    last_result = "";
    error_log = "";

    // Obtain text from php call
    std::string text = params[0];

    // We instantiate modules here so we have fresh ones for every parse
    // Makes sure we don't leak anything between parses
    // Security by design! Wow!
    std::vector<std::shared_ptr<Module>> module_inst;
    module_inst.reserve(modules.size() + 1);

    // Add the stdlib
    module_inst.push_back(std::make_shared<StdLib>());

    // Add php modules
    for (auto m : modules)
    {
        ;
        module_inst.push_back(std::make_shared<PhpModule>(m));
    }

    // Instanciate an implementation, transfer access to the modules
    Implementation implementation(module_inst);

    // Go through the processing of the input string
    ErrorReporter error_reporter;

    // Token scan
    Scanner scanner(text, error_reporter);
    vector<Token> tokens = scanner.ScanTokens();

    // Parse tokens
    Parser parser(tokens, error_reporter);
    shared_ptr<const Expr> ast = parser.Parse();

    // Create the interpreter
    Interpreter interpreter(std::make_shared<Implementation>(implementation), error_reporter);
    Value output = interpreter.Evaluate(ast);

    // No return but value set (with force to string)
    // Discourages use without error check
    last_result = output.GetContent();

    process_errors(error_reporter);
}

void MarTeX::process_errors(ErrorReporter &reporter)
{
    // Get's called once per parse
    // Included for future additions
    // Maybe source error highlighting info?

    error_log = reporter.Output();
    has_error = reporter.HadError();
    reporter.ResetErrorFlag();
}

Php::Value MarTeX::HasError()
{
    return has_error;
}

Php::Value MarTeX::GetErrors()
{
    return error_log;
}

Php::Value MarTeX::GetResult()
{
    return last_result;
}