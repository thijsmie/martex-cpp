#include "stdlib.hpp"

StdLib::StdLib() {}

std::vector<std::string> StdLib::GetEnvs()
{
    return std::vector<std::string>();
}

std::vector<std::string> StdLib::GetGlobals()
{
    return {"section", "subsection", "subsubsection",
            "textbf", "textit", "underline", "hline",
            "smallcaps", "newline", "title",
            "func", "ss",
            "copy", "cdot", "pm", "euro",
            "pound", "tm", "deg", "backslash",
            "Alpha", "alpha", "Beta", "beta",
            "Gamma", "gamma", "Delta", "delta",
            "Epsilon", "epsilon", "Zeta", "zeta",
            "Eta", "eta", "Theta", "theta",
            "Iota", "iota", "Kappa", "kappa",
            "Lambda", "lambda", "Mu", "mu",
            "Nu", "nu", "Xi", "xi",
            "Omicron", "omicron", "Pi", "pi",
            "Rho", "rho", "Sigma", "sigma",
            "Tau", "tau", "Upsilon", "upsilon",
            "Phi", "phi", "Omega", "omega"};
}

std::shared_ptr<Environment> StdLib::MakeEnv(std::string, std::shared_ptr<Environment>)
{
    /// This is nothing, cannot be called
    return nullptr;
}

Value StdLib::RunGlobal(std::shared_ptr<Environment>, Token cmd, std::vector<Value> args)
{
    static const std::vector<std::string> easy_replace(
        {"func", "ss", "copy", "euro",
         "pound", "deg",
         "Alpha", "alpha", "Beta", "beta",
         "Gamma", "gamma", "Delta", "delta",
         "Epsilon", "epsilon", "Zeta", "zeta",
         "Eta", "eta", "Theta", "theta",
         "Iota", "iota", "Kappa", "kappa",
         "Lambda", "lambda", "Mu", "mu",
         "Nu", "nu", "Xi", "xi",
         "Omicron", "omicron", "Pi", "pi",
         "Rho", "rho", "Sigma", "sigma",
         "Tau", "tau", "Upsilon", "upsilon",
         "Phi", "phi", "Omega", "omega"});
    static const std::map<std::string, std::string> full_replace(
        {{"func", "&fnof;"},
         {"cdot", "&middot;"},
         {"tm", "&trade;"},
         {"hline", "<hr>"},
         {"ss", "&szlig;"},
         {"newline", "<br>"},
         {"backslash", "&#92;"}});
    static const std::vector<std::string> functs(
        {"section", "subsection", "subsubsection",
         "textbf", "textit", "underline",
         "smallcaps", "newline", "title"});

    std::string command = cmd.GetLexeme();

    if (std::find(easy_replace.begin(), easy_replace.end(), command) != easy_replace.end())
    {
        return Value(t_string, "&" + command + ";");
    }

    if (full_replace.find(command) != full_replace.end())
    {
        return Value(t_string, full_replace.at(command));
    }

    if (args.size() != 1)
        throw RuntimeError(cmd, "One argument expected");

    switch ((int)std::distance(functs.begin(), std::find(functs.begin(), functs.end(), command)))
    {
    case 0: // section
        return Value("h3", args);
    case 1: // subsection
        return Value("h4", args);
    case 2: // subsubsection
        return Value("h5", args);
    case 3: // textbf
        return Value("b", args);
    case 4: // textit
        return Value("i", args);
    case 5: // underline
        return Value("u", args);
    case 6: // smallcaps
        return Value("span", {Value("style", "font-variant: small-caps;"), args[0]});
    case 7: // title
        return Value("h2", args);
    }
    return Value();
}
