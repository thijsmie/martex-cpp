#include "stdlib.hpp"

#include "document.hpp"
#include "figure.hpp"
#include "itemize.hpp"
#include "page.hpp"
#include "tabular.hpp"
#include "textual.hpp"

#include "core/util/regex_util.hpp"

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
     {"backslash", "&#92;"},
     {"reg", "&#174;"}});

StdLib::StdLib(bool adminmode)
{
    m_haspage = false;
    m_hasdocument = false;
    m_adminmode = adminmode;

    // Easy
    for (const auto &s : easy_replace)
        AddMethod(s, &StdLib::easy);

    // Full
    for (const auto &s : full_replace)
        AddMethod(s.first, &StdLib::full);

    // Functions
    AddMethod("textbf", &StdLib::textbf);
    AddMethod("textit", &StdLib::textit);
    AddMethod("textul", &StdLib::textul);
    AddMethod("textsc", &StdLib::textsc);
    AddMethod("textms", &StdLib::textms);
    
    AddMethod("bold", &StdLib::textbf);
    AddMethod("italic", &StdLib::textit);
    AddMethod("underline", &StdLib::textul);
    AddMethod("smallcaps", &StdLib::textsc);
    AddMethod("mono", &StdLib::textms);

    AddMethod("ref", &StdLib::ref);
    AddMethod("newline", &StdLib::newline);

    AddMethod("command", &StdLib::command);
    AddMethod("href", &StdLib::href);
    AddMethod("define", &StdLib::define);

    AddMethod("colour", &StdLib::colour);
    AddMethod("color", &StdLib::colour);
}

std::vector<std::string> StdLib::GetEnvs()
{
    return {"itemize", "enumerate", "paragraph", "tabular", "figure", "code", "center", "page", "document"};
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
    if (name == "figure")
        return std::make_shared<FigureEnvironment>(parent);
    if (name == "code")
        return std::make_shared<CodeEnvironment>(parent);
    if (name == "center")
        return std::make_shared<CenterEnvironment>(parent);
    if (!m_hasdocument && name == "document")
    {
        m_hasdocument = true;
        return std::make_shared<DocumentEnvironment>(parent, m_adminmode);
    }
    if (!m_haspage && m_adminmode && name == "page")
    {
        m_haspage = true;
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
    return Value("span", std::move(args));
}

Value StdLib::textms(std::shared_ptr<Environment>, Token cmd, std::vector<Value> args)
{
    if (args.size() != 1)
        throw RuntimeError(cmd, "takes one argument");

    args.emplace(args.begin(), "class", "martex-mono");

    return Value("span", std::move(args));
}

Value StdLib::newline(std::shared_ptr<Environment>, Token, std::vector<Value>)
{
    return Value("br", std::vector<Value>());
}

Value StdLib::ref(std::shared_ptr<Environment> runenv, Token cmd, std::vector<Value> args)
{
    return Value(t_string, runenv->Get(Token(WORD, std::move(args)[0].GetContent(), cmd.GetLine())).GetContent());
}

Value StdLib::command(std::shared_ptr<Environment>, Token, std::vector<Value> args)
{
    if (args.size() == 1)
    {
        args.emplace(args.begin(), t_string, "&#92;");
        args.emplace(args.begin(), "class", "martex-mono");
        return Value("span", std::move(args));
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
        args.emplace(args.begin(), "class", "martex-mono");
        return Value("span", std::move(args));
    }
}

Value StdLib::href(std::shared_ptr<Environment>, Token cmd, std::vector<Value> args)
{
    if (args.size() != 2)
        throw RuntimeError(cmd, "takes two arguments");

    std::string url = args[0].GetContent();

    if (!util::is_valid_url(url) && !util::is_valid_subpath(url))
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

