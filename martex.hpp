#include "interpreter.hpp"
#include "implementation.hpp"

#include <phpcpp.h>

#include<map>
#include<unordered_set>
#include<vector>
#include<memory>
#include<string>

class PhpEnvironment : public Environment
{
  private:
    std::unordered_set<std::string> functions;
    Php::Object MyPhpCounterpart;
    
  public:
    PhpHtmlEnvironment(std::string);

    bool HasCommand(std::string);
    Value RunCommandHere(std::shared_ptr<Environment>, std::string, std::vector<Value>);
    void StartEnvironment(Value, std::vector<Value>);
    void EndEnvironment(Value);
};

class PhpHtmlImplementation : public Implementation
{
  private:


  public:
    std::shared_ptr<Environment> Global();
    std::shared_ptr<Environment> Create(Token name, std::shared_ptr<Environment> parent);
    std::string Escaped(TokenType escapetype, char escapee);
    std::string LineBreak() const;
    std::string Ampersand() const;
};

class PhpHtmlModule
{
  private:
    std::unordered_set<std::string> functions;
    std::string envname;

}

class MarTeX : Php::Base
{
  private:
    std::unordered_set<
  public:
    void RegisterModule(Php::Parameters &params);
    void Parse(Php::Parameters &params);
}