#include "stdlib.hpp"
#include "itemize.hpp"
#include "document.hpp"

StdLib::StdLib(bool allow_page)
{
    chapter = 0;
    section = 0;
    subsection = 0;
    subsubsection = 0;
    labeling = false;

    haspage = !allow_page;
    hasdocument = !allow_page;
}

std::vector<std::string> StdLib::GetEnvs()
{
    return std::vector<std::string>{"itemize", "enumerate", "paragraph", "page", "document"};
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
            "ref", "label", "labeling", "command", "href"};
}

std::shared_ptr<Environment> StdLib::MakeEnv(std::string name, std::shared_ptr<Environment> parent)
{
    if (name == "itemize")
        return std::make_shared<ItemizeEnvironment>(parent);
    if (name == "enumerate")
        return std::make_shared<EnumerateEnvironment>(parent);
    if (name == "paragraph")
        return std::make_shared<ParagraphEnvironment>(parent);
    if (!hasdocument && name == "document")
    {
        hasdocument = true;
        return std::make_shared<DocumentEnvironment>(parent);
    }
    if (!haspage && name == "page")
    {
        haspage = true;
        return std::make_shared<PageEnvironment>(parent);
    }
    throw RuntimeError(Token(BEGIN_ENV, "", -1), "Env not allowed");
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
         "labeling", "ref", "label", "command"});

    std::string command = cmd.GetLexeme();

    if (args.size() == 0)
    {
        if (std::find(easy_replace.begin(), easy_replace.end(), command) != easy_replace.end())
        {
            return Value(t_string, "&" + command + ";");
        }

        if (full_replace.find(command) != full_replace.end())
        {
            return Value(t_string, full_replace.at(command));
        }
    }
    else if (args.size() == 1)
    {
        switch ((int)std::distance(functs.begin(), std::find(functs.begin(), functs.end(), command)))
        {
        case 0: // section
        {
            section++;
            subsection = 0;
            subsubsection = 0;
            if (labeling)
                args.emplace(args.begin(), t_string, std::to_string(section) + "&emsp;");

            return Value("h3", std::move(args));
        }
        case 1: // subsection
        {
            subsection++;
            subsubsection = 0;
            if (labeling)
                args.emplace(args.begin(), t_string,
                             std::to_string(section) + "." + std::to_string(subsection) + "&emsp;");

            return Value("h4", std::move(args));
        }
        case 2: // subsubsection
        {
            subsubsection++;
            if (labeling)
                args.emplace(args.begin(), t_string,
                             std::to_string(section) + "." + std::to_string(subsection) +
                                 std::to_string(subsubsection) + "&emsp;");

            return Value("h5", std::move(args));
        }
        case 3: // textbf
            return Value("b", std::move(args));
        case 4: // textit
            return Value("i", std::move(args));
        case 5: // underline
            return Value("u", std::move(args));
        case 6: // smallcaps
            args.emplace(args.begin(), "style", "font-variant: small-caps;");
            return Value("span", std::move(args[0]));
        case 7: // newline
            return Value("br", std::vector<Value>());
        case 8: // title
            return Value("h1", std::move(args));
        case 9: // chapter
        {
            chapter++;
            section = 0;
            subsection = 0;
            subsubsection = 0;
            if (labeling)
                args.emplace(args.begin(), t_string, std::to_string(chapter) + "&emsp;");

            return Value("h2", std::move(args));
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
            return Value(t_string, runenv->Get(Token(WORD, std::move(args)[0].GetContent(), cmd.GetLine())).GetContent());
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
        case 13: // command
        {
            args.emplace(args.begin(), t_string, "&#92;");
            return Value(std::move(args));
        }
        }
    }
    else
    {
        if (command == "command")
        {
            args.reserve(args.size() + (args.size() - 1) * 2 + 1);

            for (int i = args.size() - 1; i > 0; i--)
            {
                args.emplace(args.begin() + i + 1, t_string, "&#125;");
                args.emplace(args.begin() + i, t_string, "&#123;");
            }

            args.emplace(args.begin(), t_string, "&#92;");
            return Value(std::move(args));
        }
        else if (command == "href")
        {
            if (args.size() != 2)
                throw RuntimeError(cmd, "href takes two arguments");

            args[0] = Value("href", args[0].GetContent());

            return Value("a", std::move(args));
        }
    }
    throw RuntimeError(cmd, "This command does not exist?");
}
