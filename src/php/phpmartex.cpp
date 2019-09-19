#include "phpmartex.hpp"

#include "phpmodule.hpp"
#include "phpenvironment.hpp"

#include "core/language/transpiler.hpp"
#include "core/modules/stdlib.hpp"
#include "core/util/plain.hpp"

using std::shared_ptr;
using std::string;
using std::vector;

MarTeX::MarTeX() : has_error(false), allow_page(false), last_result(""), error_log(""), t(nullptr) {}

void MarTeX::RegisterModule(Php::Parameters &params)
{
    string module_name = params[0];
    modules.push_back(module_name);
}

void MarTeX::AllowPage()
{
    allow_page = true;
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
    module_inst.push_back(std::make_shared<StdLib>(allow_page));

    // Add php modules
    for (auto m : modules)
    {
        module_inst.push_back(std::make_shared<PhpModule>(m));
    }

    // Run the transpiler
    if (t != nullptr)
        free(t);

    t = new Transpiler(module_inst);
    transpiler.Parse(text);

    has_error = transpiler.HasError();
    error_log = transpiler.GetErrors();
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
    return t->GetResult();
}

Php::Value MarTeX::GetPlainResult()
{
    return ToPlain(t->GetResultRaw());
}