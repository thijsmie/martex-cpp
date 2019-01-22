#pragma once

#include "core/language/environment.hpp"

#include <phpcpp.h>

#include <vector>
#include <map>
#include <string>
#include <memory>

enum ArgType : int
{
    NoArg,
    Full,
    Text,
    Bytes,
    PlainFull,
    PlainText,
    PlainBytes,
    MoreFull,
    MoreText,
    MoreBytes,
    MorePlainFull,
    MorePlainText,
    MorePlainBytes
};

std::vector<ArgType> ParseSignature(Php::Value signature);
Php::Value ValidateSignature(std::vector<ArgType> types, const std::vector<Value> &arguments);
Php::Value ValidateSignature(ArgType type, const Value &arguments);

class PhpEnvironment : public Environment
{
  private:
    std::vector<std::string> commands;
    std::map<std::string, std::vector<ArgType>> callsigns;
    ArgType BeginBracketArg;

  public:
    Php::Object myEnvironment;

    PhpEnvironment(std::string, Php::Object, Php::Object, std::shared_ptr<Environment>);
    bool HasCommand(std::string);
    Value RunCommandHere(std::shared_ptr<Environment>, Token, std::vector<Value>);
    void StartEnvironment(Token, Value);
    Value EndEnvironment(Token, Value);
};

/// Base class for php environments to inherit
class PhpEnvironmentBase : public Php::Base
{
  private:
    std::shared_ptr<PhpEnvironment> RealMe;

  public:
    void __construct(Php::Parameters &params)
    {
        Php::Value self(this);
        self["module"] = params[0];
        self["tex"] = params[1];
    }

    Php::Value locals()
    {
        return Php::Array();
    }

    void begin(Php::Parameters &)
    {
    }

    Php::Value end(Php::Parameters &)
    {
        return Php::Value();
    }
};