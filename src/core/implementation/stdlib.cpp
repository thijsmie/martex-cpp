#include "stdlib.hpp"
#include "itemize.hpp"
#include "document.hpp"
#include "tabular.hpp"

#include "core/util/uri_util.hpp"

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

StdLib::StdLib(bool allow_page)
{
    _chapter = 0;
    _section = 0;
    _subsection = 0;
    _subsubsection = 0;
    _labeling = false;

    _haspage = !allow_page;
    _hasdocument = !allow_page;

    // Easy
    for (const auto &s : easy_replace)
        AddMethod(s, &StdLib::easy);

    // Full
    for (const auto &s : full_replace)
        AddMethod(s.first, &StdLib::full);

    // Functions
    AddMethod("title", &StdLib::title);
    AddMethod("chapter", &StdLib::chapter);
    AddMethod("section", &StdLib::section);
    AddMethod("subsection", &StdLib::subsection);
    AddMethod("subsubsection", &StdLib::subsubsection);

    AddMethod("textbf", &StdLib::textbf);
    AddMethod("textit", &StdLib::textit);
    AddMethod("textul", &StdLib::textul);
    AddMethod("underline", &StdLib::textul);
    AddMethod("textsc", &StdLib::textsc);
    AddMethod("smallcaps", &StdLib::textsc);

    AddMethod("ref", &StdLib::ref);
    AddMethod("label", &StdLib::label);
    AddMethod("labeling", &StdLib::labeling);
    AddMethod("newline", &StdLib::newline);

    AddMethod("command", &StdLib::command);
    AddMethod("href", &StdLib::href);
    AddMethod("define", &StdLib::define);

    AddMethod("colour", &StdLib::colour);
    AddMethod("color", &StdLib::colour);
}

std::vector<std::string> StdLib::GetEnvs()
{
    return std::vector<std::string>{"itemize", "enumerate", "paragraph", "page", "document", "tabular"};
}

std::shared_ptr<Environment> StdLib::MakeEnv(std::string name, std::shared_ptr<Environment> parent)
{
    if (name == "itemize")
        return std::make_shared<ItemizeEnvironment>(parent);
    if (name == "enumerate")
        return std::make_shared<EnumerateEnvironment>(parent);
    if (name == "paragraph")
        return std::make_shared<ParagraphEnvironment>(parent);
    if (name == "tabular")
        return std::make_shared<TabularEnvironment>(parent);
    if (!_hasdocument && name == "document")
    {
        _hasdocument = true;
        return std::make_shared<DocumentEnvironment>(parent);
    }
    if (!_haspage && name == "page")
    {
        _haspage = true;
        return std::make_shared<PageEnvironment>(parent);
    }
    throw RuntimeError(Token(BEGIN_ENV, "", -1), "Env not allowed");
}

Value StdLib::easy(std::shared_ptr<Environment>, Token cmd, std::vector<Value> args)
{
    if (args.size() > 0)
        throw RuntimeError(cmd, "takes no arguments");

    std::string command = cmd.GetLexeme();
    return Value(t_string, "&" + command + ";");
}

Value StdLib::full(std::shared_ptr<Environment>, Token cmd, std::vector<Value> args)
{
    if (args.size() > 0)
        throw RuntimeError(cmd, "takes no arguments");

    std::string command = cmd.GetLexeme();
    return Value(t_string, full_replace.at(command));
}

/// full commands
Value StdLib::title(std::shared_ptr<Environment>, Token, std::vector<Value> args)
{
    return Value("h1", std::move(args));
}

Value StdLib::chapter(std::shared_ptr<Environment>, Token, std::vector<Value> args)
{
    _chapter++;
    _section = 0;
    _subsection = 0;
    _subsubsection = 0;

    if (_labeling)
        args.emplace(args.begin(), t_string, std::to_string(_chapter) + "&emsp;");

    return Value("h2", std::move(args));
}

Value StdLib::section(std::shared_ptr<Environment>, Token, std::vector<Value> args)
{
    _section++;
    _subsection = 0;
    _subsubsection = 0;
    if (_labeling)
        args.emplace(args.begin(), t_string, std::to_string(_section) + "&emsp;");

    return Value("h3", std::move(args));
}

Value StdLib::subsection(std::shared_ptr<Environment>, Token, std::vector<Value> args)
{
    _subsection++;
    _subsubsection = 0;
    if (_labeling)
        args.emplace(args.begin(), t_string,
                     std::to_string(_section) + "." + std::to_string(_subsection) + "&emsp;");

    return Value("h4", std::move(args));
}

Value StdLib::subsubsection(std::shared_ptr<Environment>, Token, std::vector<Value> args)
{
    _subsubsection++;
    if (_labeling)
        args.emplace(args.begin(), t_string,
                     std::to_string(_section) + "." + std::to_string(_subsection) +
                         std::to_string(_subsubsection) + "&emsp;");

    return Value("h5", std::move(args));
}

Value StdLib::textbf(std::shared_ptr<Environment>, Token, std::vector<Value> args)
{
    return Value("b", std::move(args));
}

Value StdLib::textit(std::shared_ptr<Environment>, Token, std::vector<Value> args)
{
    return Value("i", std::move(args));
}

Value StdLib::textul(std::shared_ptr<Environment>, Token, std::vector<Value> args)
{
    return Value("u", std::move(args));
}

Value StdLib::textsc(std::shared_ptr<Environment>, Token, std::vector<Value> args)
{
    args.emplace(args.begin(), "style", "font-variant: small-caps;");
    return Value("span", std::move(args[0]));
}

Value StdLib::newline(std::shared_ptr<Environment>, Token, std::vector<Value>)
{
    return Value("br", std::vector<Value>());
}

Value StdLib::labeling(std::shared_ptr<Environment>, Token, std::vector<Value> args)
{
    std::string a = args[0].GetContent();

    if (a == "true" || a == "1")
        _labeling = true;
    else
        _labeling = false;

    return Value();
}

Value StdLib::ref(std::shared_ptr<Environment> runenv, Token cmd, std::vector<Value> args)
{
    return Value(t_string, runenv->Get(Token(WORD, std::move(args)[0].GetContent(), cmd.GetLine())).GetContent());
}

Value StdLib::label(std::shared_ptr<Environment> runenv, Token, std::vector<Value> args)
{
    std::string l = "";
    if (_subsubsection > 0)
        l = std::to_string(_section) + "." + std::to_string(_subsection) + "." + std::to_string(_subsubsection);
    else if (_subsection > 0)
        l = std::to_string(_section) + "." + std::to_string(_subsection);
    else if (_section > 0)
        l = std::to_string(_section);
    else
        l = std::to_string(_chapter);

    while (!runenv->IsRoot())
        runenv = runenv->GetEnclosing();

    runenv->Set(args[0].GetContent(), Value(t_string, l));
    return Value();
}

Value StdLib::command(std::shared_ptr<Environment>, Token, std::vector<Value> args)
{
    if (args.size() == 1)
    {
        args.emplace(args.begin(), t_string, "&#92;");
        return Value(std::move(args));
    }
    else
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
}

Value StdLib::href(std::shared_ptr<Environment>, Token cmd, std::vector<Value> args)
{
    if (args.size() != 2)
        throw RuntimeError(cmd, "takes two arguments");

    std::string url = args[0].GetContent();

    if (!util::is_valid_url(url))
        throw RuntimeError(cmd, "Invalid URL");

    args[0] = Value("href", url);

    return Value("a", std::move(args));
}

Value StdLib::define(std::shared_ptr<Environment> runenv, Token cmd, std::vector<Value> args)
{
    if (args.size() != 2)
        throw RuntimeError(cmd, "takes two arguments");

    if (!args[0].IsPlain())
        throw RuntimeError(cmd, "name should be plaintext");

    std::string name = args[0].GetContent();
    util::trim(name);

    runenv->Set(name, std::move(args[1]));
    return Value();
}

Value StdLib::colour(std::shared_ptr<Environment>, Token cmd, std::vector<Value> args)
{
    if (args.size() != 2)
        throw RuntimeError(cmd, "takes two arguments");

    std::string col = args[0].GetContent();
    util::trim(col);

    if (col.size() != 6 || !util::hexonly(col))
        throw RuntimeError(cmd, "invalid hexcode");

    args[0] = Value("style", "color: #" + col);

    return Value("span", std::move(args));
}