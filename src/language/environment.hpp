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
    std::shared_ptr<Environment> enclosing;

  public:
    bool IsRoot() { return is_root; }
    std::shared_ptr<Environment> GetEnclosing() { return enclosing; }
    void Set(std::string, Value);
    void SetGlobal(std::string, Value);
    Value& Get(Token);
    Value& Get(std::string);
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