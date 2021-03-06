#pragma once

#include <memory>
#include <map>
#include "runtime_error.hpp"
#include "value.hpp"
#include "token.hpp"

class Environment
{
  protected:
    bool is_root;
    std::map<std::string, Value> values;
    std::map<std::string, std::string> stringvalues;
    std::shared_ptr<Environment> enclosing;

  public:
    Environment() {} 
    Environment(std::shared_ptr<Environment> p) : is_root(false), enclosing(p) {}

    bool IsRoot() { return is_root; }
    std::shared_ptr<Environment> GetEnclosing() { return enclosing; }

    void Set(std::string, Value);
    void SetGlobal(std::string, Value);
    Value& Get(Token);
    Value& Get(std::string);

    void SetString(std::string, std::string);
    void SetGlobalString(std::string, std::string);
    std::string GetString(std::string);
    bool HasString(std::string);

    Value RunCommand(std::shared_ptr<Environment>, Token, std::vector<Value>);

    virtual bool HasCommand(std::string) = 0;
    virtual Value RunCommandHere(std::shared_ptr<Environment>, Token, std::vector<Value>) = 0;
    virtual void StartEnvironment(Token, Value) = 0;
    virtual Value EndEnvironment(Token, Value) = 0;
};

class EmptyEnvironment : public Environment
{
  public:
    EmptyEnvironment(std::shared_ptr<Environment> parent) 
    {
      is_root = false;
      enclosing = parent;
    }

    bool HasCommand(std::string) {return false;}
    Value RunCommandHere(std::shared_ptr<Environment>, Token, std::vector<Value>) {return Value();}
    void StartEnvironment(Token, Value) {}
    Value EndEnvironment(Token, Value c) { return std::move(c); }
};