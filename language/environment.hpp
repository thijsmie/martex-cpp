#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <memory>
#include <unordered_map>
#include "value.hpp"

class Environment
{
    bool is_root;
    std::unordered_map<std::string, Value> values;
    std::shared_ptr<Environment> enclosing;

    

  public:
    Environment();
    Environment(std::string, std::shared_ptr<Environment>);

    void Set(std::string, Value);
    Value Get(Token);
    Value RunCommand(Token, std::vector<Value>);
    Value RunCommand(Environment, Token, std::vector<Value>);
    
    virtual bool HasCommand(std::string) = 0;
    virtual Value RunCommandHere(std::shared_ptr<Environment>, std::string, std::vector<Value>) = 0;
    virtual void StartEnvironment(Value, std::vector<Value>) = 0;
    virtual void EndEnvironment(Value) = 0;
};

#endif // ENVIRONMENT_H