#include "transpiler.hpp"

#include "token.hpp"
#include "expr.hpp"
    
#include "scanner.hpp"
#include "parser.hpp"
#include "interpreter.hpp"

#include "globalenvironment.hpp"
#include "core/implementation/implementation.hpp"

Transpiler::Transpiler(std::vector<std::shared_ptr<Module>> modules) : used(false), error_reporter(), modules(modules) {}

void Transpiler::Parse(std::string text)
{
    if (used) {
        error_reporter.Log("Tried to reuse transpiler.");
        return;
    }

    // set used, because only one use is allowed
    used = true;

    // Create Global Environment
    std::shared_ptr<GlobalEnv> globalEnv = std::make_shared<GlobalEnv>(modules);

    // Token scan
    Scanner scanner(text, error_reporter);
    std::vector<Token> tokens = scanner.ScanTokens();

    // Parse tokens
    Parser parser(tokens, error_reporter);
    std::shared_ptr<const Expr> ast = parser.Parse();

    // Create the interpreter
    Interpreter interpreter(globalEnv, std::make_shared<Implementation>(modules), error_reporter);
    result = interpreter.Evaluate(ast);
}

bool Transpiler::HasError()
{
    return error_reporter.HadError();
}

std::string Transpiler::GetErrors()
{
    return error_reporter.Output();
}

std::string Transpiler::GetResult()
{
    return result.GetContent();
}

std::string Transpiler::GetResultBytes()
{
    uint32_t size = result.ByteSize();
    unsigned char *buffer = (unsigned char*) malloc(size);

    uint32_t pos = 0;
    result.WriteOut(buffer, pos);

    std::string res((char*)buffer, size);
    free(buffer);
    return res;
}