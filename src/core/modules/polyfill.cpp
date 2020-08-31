#include "polyfill.hpp"


PolyfillLib::PolyfillLib() 
{
    AddMethod("html", &PolyfillLib::html);
    AddMethod("attr", &PolyfillLib::attr);
}

std::vector<std::string> PolyfillLib::GetEnvs()
{
    return {};
}

std::shared_ptr<Environment> PolyfillLib::MakeEnv(std::string name, std::shared_ptr<Environment> parent)
{
    throw RuntimeError(Token(BEGIN_ENV, "", -1), "Impossible to get here.");
}

Value PolyfillLib::html(std::shared_ptr<Environment>, Token cmd, std::vector<Value> args)
{
    if (args.size() != 2)
        throw RuntimeError(cmd, "takes two arguments");

    std::vector<Value> mult;
    if (args[1].GetType() != t_multi)
        mult.push_back(std::move(args[1]));
    else
        mult = std::move(args[1].multicontent);

    return Value(t_html, args[0].GetContent(), std::move(mult));
}

Value PolyfillLib::attr(std::shared_ptr<Environment>, Token cmd, std::vector<Value> args)
{
    if (args.size() != 2)
        throw RuntimeError(cmd, "takes two arguments");

    return Value(args[0].GetContent(), args[1].GetContent());
}