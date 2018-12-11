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
        return Value({Value(t_string, "<h3>"), args[0], Value(t_string, "</h3>")});
    case 1: // subsection
        return Value({Value(t_string, "<h4>"), args[0], Value(t_string, "</h4>")});
    case 2: // subsubsection
        return Value({Value(t_string, "<h5>"), args[0], Value(t_string, "</h5>")});
    case 3: // textbf
        return Value({Value(t_string, "<b>"), args[0], Value(t_string, "</b>")});
    case 4: // textit
        return Value({Value(t_string, "<i>"), args[0], Value(t_string, "</i>")});
    case 5: // underline
        return Value({Value(t_string, "<u>"), args[0], Value(t_string, "</u>")});
    case 6: // smallcaps
        return Value({Value(t_string, "<span style='font-variant: small-caps;'>"),
                      args[0], Value(t_string, "</span>")});
    case 7: // title
        return Value({Value(t_string, "<u>"), args[0], Value(t_string, "</u>")});
    }
    return Value();
}
