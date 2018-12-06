#include "martex.hpp"

#include "php/phpmodule.hpp"
#include "language/scanner.hpp"
#include "language/parser.hpp"

using std::string;
using std::vector;
using std::shared_ptr;


MarTeX::MarTeX() {}

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
    module_inst.reserve(modules.size());
    for (auto m : modules)
        module_inst.push_back(std::make_shared<Module>(PhpModule(m)));

    // Instanciate an implementation, transfer access to the modules
    Implementation implementation(module_inst);

    // Go through the processing of the input string
    ErrorReporter error_reporter;

    // Token scan
    Scanner scanner(text, error_reporter);
    vector<Token> tokens = scanner.ScanTokens();

    // Fatal error escape
    if (error_reporter.IsFatal()) {
        process_errors(error_reporter);
        return;
    }
    
    // Parse tokens
    Parser parser(tokens, error_reporter);
    vector<shared_ptr<const Expr>> ast = parser.Parse();

    // Fatal error escape
    if (error_reporter.IsFatal()) {
        process_errors(error_reporter);
        return;
    }

    // Create the interpreter
    Interpreter interpreter(std::make_shared<Implementation>(implementation), error_reporter);
    Value output = interpreter.Evaluate(ast);

    // Fatal error escape
    if (error_reporter.IsFatal()) {
        process_errors(error_reporter);
        return;
    }

    // No return but value set (with force to string)
    // Discourages use without error check
    result = output.GetContent();

    process_errors(error_reporter);
}

void MarTeX::process_errors(ErrorReporter &reporter) 
{
    // Get's called once per parse
    // Included for future additions
    // Maybe source error highlighting info?

    error_log = reporter.elog();
}