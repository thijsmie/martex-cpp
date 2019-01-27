#include <string>
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <regex>
#include <cstddef>
#include <cstdlib>

#include "core/modules/stdlib.hpp"
#include "core/language/transpiler.hpp"

std::string do_replace(std::string const &in, std::string const &from, std::string const &to)
{
    return std::regex_replace(in, std::regex(from), to);
}

std::string _compile(std::string text)
{
    std::vector<std::shared_ptr<Module>> module_inst;
    module_inst.reserve(1);
    module_inst.push_back(std::make_shared<StdLib>(false));

    // Run the transpiler
    Transpiler transpiler(module_inst);
    transpiler.Parse(text);

    std::string result = "";

    if (transpiler.HasError())
    {
        result += "<span style='color:red'>";
        result += do_replace(transpiler.GetErrors(), "\n", "<br>");
        result += "</span>";
    }
    result += transpiler.GetResult();
    return result;
}

EMSCRIPTEN_BINDINGS(martex)
{
    emscripten::function("_compile", &_compile);
}
