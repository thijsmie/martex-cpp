#include "phpenvironment.hpp"
#include <algorithm>
#include "phpvaluecasts.hpp"
#include "phpmartex.hpp"

PhpEnvironment::PhpEnvironment(std::string phpEnvName, Php::Object myModule, Php::Object GlobalEnv, std::shared_ptr<Environment> parent) : myEnvironment(phpEnvName.c_str(), myModule, GlobalEnv)
{
    Php::Value funcs = myEnvironment.call("locals");

    /// Cast, fingers crossed
    commands = funcs;

    /// Obtain function signatures
    for (const std::string &cmd : commands)
    {
        callsigns[cmd] = {MoreFull};
        try
        {
            if (myEnvironment.contains("argsfor_" + cmd))
                callsigns[cmd] = ParseSignature(myEnvironment["argsfor_" + cmd]);
        }
        catch (Php::Exception &e)
        {
        }
    }

    /// Bracket signature
    BeginBracketArg = NoArg;
    try
    {
        if (myEnvironment.contains("argsfor_begin"))
            BeginBracketArg = (ArgType)(int)myEnvironment["argsfor_begin"];
    }
    catch (Php::Exception &e)
    {
    }

    /// End content signature
    callsigns["end"] = {MoreFull};
    try
    {
        if (myEnvironment.contains("argsfor_end"))
            callsigns["end"] = ParseSignature(myEnvironment["argsfor_end"]);
    }
    catch (Php::Exception &e)
    {
    }

    is_root = false;
    enclosing = parent;
}

bool PhpEnvironment::HasCommand(std::string c)
{
    return (std::find(commands.begin(), commands.end(), c) != commands.end());
}

Value PhpEnvironment::RunCommandHere(std::shared_ptr<Environment> env, Token name, std::vector<Value> args)
{
    std::shared_ptr<PhpEnvironment> phpenv = std::dynamic_pointer_cast<PhpEnvironment>(env);
    try
    {
        Php::Value arguments = ValidateSignature(callsigns[name.GetLexeme()], args);
        return PhpToCpp(myEnvironment.call(name.GetLexeme().c_str(), (Php::Value)phpenv->myEnvironment, arguments));
    }
    catch (Php::Exception &e)
    {
        throw RuntimeError(name, e.message());
    }
}

void PhpEnvironment::StartEnvironment(Token begin, Value arg)
{
    try
    {
        Php::Value argument;
        switch (BeginBracketArg)
        {
        case PlainFull:
            if (!arg.IsPlain())
                throw Php::Exception("expected plain value");
        case Full:
            argument = CppToPhp(arg);
            break;
        case PlainText:
            if (!arg.IsPlain())
                throw Php::Exception("expected plain value");
        case Text:
            argument = arg.GetContent();
            break;
        case PlainBytes:
            if (!arg.IsPlain())
                throw Php::Exception("expected plain value");
        case Bytes:
            argument = CppToPhpBytes(arg);
            break;
        case NoArg:
            break;
        default:
            // Note, we shouldn't have any other types in here
            throw Php::Exception("Uhm?");
        }
        myEnvironment.call("begin", argument);
    }
    catch (Php::Exception &e)
    {
        throw RuntimeError(Token(BEGIN_ENV, begin.GetLexeme(), begin.GetLine()), e.message());
    }
}

Value PhpEnvironment::EndEnvironment(Token end, Value content)
{
    try
    {
        Php::Value phpcont = ValidateSignature(callsigns["end"], std::vector<Value>{content})[0];
        Php::Value res = myEnvironment.call("end", phpcont);
        if (res.isNull())
            return content;
        return PhpToCpp(res);
    }
    catch (Php::Exception &e)
    {
        throw RuntimeError(Token(END_ENV, end.GetLexeme(), end.GetLine()), e.message());
    }
}

std::vector<ArgType> ParseSignature(Php::Value signature)
{
    std::vector<ArgType> res;

    for (auto &pv : signature)
    {
        ArgType t = (ArgType)(int)pv.second;
        res.push_back(t);
    }

    for (int i = 0; i < res.size(); i++)
        switch (res[i])
        {
        case Full:
        case Text:
        case Bytes:
        case PlainFull:
        case PlainText:
        case PlainBytes:
            // these are fine everywhere
            break;
        case MoreFull:
        case MoreText:
        case MoreBytes:
        case MorePlainFull:
        case MorePlainText:
        case MorePlainBytes:
            // these are only fine as last argument
            if (i != res.size() - 1)
                throw Php::Exception("Invalid type signature");
            break;
        default:
            throw Php::Exception("Type signature contained invalid values.");
        }
    return res;
}

Php::Value ValidateSignature(std::vector<ArgType> types, std::vector<Value> arguments)
{
    if (types.size() == 0 && arguments.size() > 0)
        throw Php::Exception("Arguments given to command that does not accept any");

    if (types.size() == 0 && arguments.size() == 0)
        return Php::Value(); // Return NULL

    // Need to validate from here

    if (arguments.size() > types.size())
    {
        // Check if function is vararguable
        ArgType last = types[types.size() - 1];

        switch (last)
        {
        case Full:
        case Text:
        case Bytes:
        case PlainFull:
        case PlainText:
        case PlainBytes:
            // nope
            throw Php::Exception("Too many arguments given to function.");
        // ok, vararguable, set type
        case MoreFull:
            last = Full;
            break;
        case MoreText:
            last = Text;
            break;
        case MoreBytes:
            last = Bytes;
            break;
        case MorePlainFull:
            last = PlainFull;
            break;
        case MorePlainText:
            last = PlainText;
            break;
        case MorePlainBytes:
            last = PlainBytes;
            break;
        }

        // Set last
        types[types.size() - 1] = last;

        // Add type from more till sizes match
        while (types.size() < arguments.size())
            types.push_back(last);
    }
    else if (arguments.size() == types.size())
    {
        // sizes match, that's great! Last one still might be a "more" type
        ArgType last = types[types.size() - 1];

        switch (last)
        {
        case Full:
        case Text:
        case Bytes:
        case PlainFull:
        case PlainText:
        case PlainBytes:
            // nope, okay
            break;
        // moretypes, need setting:
        case MoreFull:
            last = Full;
            break;
        case MoreText:
            last = Text;
            break;
        case MoreBytes:
            last = Bytes;
            break;
        case MorePlainFull:
            last = PlainFull;
            break;
        case MorePlainText:
            last = PlainText;
            break;
        case MorePlainBytes:
            last = PlainBytes;
            break;
        }

        // Set last
        types[types.size() - 1] = last;
    }
    else if (arguments.size() == types.size() - 1)
    {
        // arguments size is a bit too small, but maybe we can have an unfilled "more" type
        ArgType last = types[types.size() - 1];

        switch (last)
        {
        case Full:
        case Text:
        case Bytes:
        case PlainFull:
        case PlainText:
        case PlainBytes:
            // nope, error then!
            throw Php::Exception("Not enough arguments");
        // moretypes, drop out
        case MoreFull:
        case MoreText:
        case MoreBytes:
        case MorePlainFull:
        case MorePlainText:
        case MorePlainBytes:
            break;
        }

        /// drop the moretype
        types.pop_back();
    }
    else
    {
        /// arguments < types - 1
        throw Php::Exception("Not enough arguments");
    }

    // Types and Arguments are now equally sized
    // assert(types.size() == arguments.size());

    // Check "Plain"-ness of vars when needed

    for (int i = 0; i < types.size(); i++)
    {
        switch (types[i])
        {
        case Full:
        case Text:
        case Bytes:
            // No further requirements
            break;
        case PlainFull:
        case PlainText:
        case PlainBytes:
            if (!arguments[i].IsPlain())
                throw Php::Exception("This command takes 'plain' content only (so no html)!");
            break;
        default:
            // Note, we don't have "more" types anymore
            break;
        }
    }

    // Okay! Ready for conversion!
    Php::Array args;

    for (int i = 0; i < types.size(); i++)
    {
        switch (types[i])
        {
        case Full:
        case PlainFull:
            args[i] = CppToPhp(arguments[i]);
            break;
        case Text:
        case PlainText:
            args[i] = arguments[i].GetContent();
            break;
        case Bytes:
        case PlainBytes:
            args[i] = CppToPhpBytes(arguments[i]);
            break;
        default:
            // Note, we don't have "more" types anymore
            break;
        }
    }

    return args;
}