#include "stdlib.hpp"

StdLib::StdLib() {
    chapter = 0;
    section = 0;
    subsection = 0;
    subsubsection = 0;
    labeling = false;
}

std::vector<std::string> StdLib::GetEnvs()
{
    return std::vector<std::string>();
}

std::vector<std::string> StdLib::GetGlobals()
{
    return {"section", "subsection", "subsubsection",
            "textbf", "textit", "underline", "hline",
            "smallcaps", "newline", "title", "chapter",
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
            "Phi", "phi", "Omega", "omega",
            "ref", "label", "labeling"};
}

std::shared_ptr<Environment> StdLib::MakeEnv(std::string, std::shared_ptr<Environment>)
{
    /// This is nothing, cannot be called
    return nullptr;
}

Value StdLib::RunGlobal(std::shared_ptr<Environment> runenv, Token cmd, std::vector<Value> args)
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
         "smallcaps", "newline", "title", "chapter",
         "labeling", "ref", "label"});

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
    {
        section++;
        subsection = 0;
        subsubsection = 0;
        if (labeling)
            args.insert(args.begin(), Value(t_string, std::to_string(section) + "&emsp;"));
        
        return Value("h3", args);
    }
    case 1: // subsection
    {
        subsection++;
        subsubsection = 0;
        if (labeling)
            args.insert(args.begin(), Value(t_string, 
                std::to_string(section) + "." + std::to_string(subsection) + "&emsp;"));
        
        return Value("h4", args);
    }
    case 2: // subsubsection
    {
        subsubsection++;
        if (labeling)
            args.insert(args.begin(), Value(t_string, 
                std::to_string(section) + "." + std::to_string(subsection) +
                std::to_string(subsubsection) + "&emsp;"));
        
        return Value("h5", args);
    }
    case 3: // textbf
        return Value("b", args);
    case 4: // textit
        return Value("i", args);
    case 5: // underline
        return Value("u", args);
    case 6: // smallcaps
        return Value("span", {Value("style", "font-variant: small-caps;"), args[0]});
    case 7: // newline
        return Value("br", {});
    case 8: // title
        return Value("h1", args);
    case 9: // chapter
    {
        chapter++;
        section = 0;
        subsection = 0;
        subsubsection = 0;
        if (labeling)
            args.insert(args.begin(), Value(t_string, 
                std::to_string(chapter) + "&emsp;"));

        return Value("h2", args);
    }
    case 10: // labeling
    {
        std::string a = args[0].GetContent();
        if (a == "true" || a == "1")
            labeling = true;
        else
            labeling = false;
        return Value();
    }
    case 11: // ref
    {
        return runenv->Get(Token(WORD, args[0].GetContent(), cmd.GetLine()));
    }
    case 12: // label 
    {
        std::string l = "";
        if (subsubsection > 0)
            l = std::to_string(section) + "." + std::to_string(subsection) + "." + std::to_string(subsubsection);
        else if (subsection > 0)
            l = std::to_string(section) + "." + std::to_string(subsection);
        else if (section > 0)
            l = std::to_string(section);
        else 
            l = std::to_string(chapter);
        
        while (!runenv->IsRoot())
            runenv = runenv->GetEnclosing();

        runenv->Set(args[0].GetContent(), Value(t_string, l));
        return Value();
    }
    }
    throw RuntimeError(cmd, "This command does not exist?");    
}
