#include "interpreter.hpp"
#include "implementation.hpp"

#include <phpcpp.h>

#include <map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <string>

/// Base class for php environments to inherit
class PhpEnvironmentBase : public Php::Base
{
public:
  void __construct(Php::Parameters &params)
  {
    Php::Value self(this);
    self["tex"] = params[0];
    self["module"] = params[1];
  }
};

/// Class to use as environment in the parser
/// Holds a reference to the sister PhpEnvironmentBase inheriting php instance
class PhpEnvironment : public Environment
{
private:
  std::unordered_set<std::string> functions;
  Php::Object MyPhpCounterpart;

public:
  PhpHtmlEnvironment(std::string);

  bool HasCommand(std::string);
  Value RunCommandHere(std::shared_ptr<Environment>, std::string, std::vector<Value>);
  void StartEnvironment(Value);
  void EndEnvironment(Value);
};

/// Globals!
class PhpGlobalEnvironment : public Environment
{
private:
  std::vector<std::shared_ptr<PhpHtmlModule>> modules;
public:
  PhpGlobalEnvironment(std::string);

  bool HasCommand(std::string);
  Value RunCommandHere(std::shared_ptr<Environment>, std::string, std::vector<Value>);
  void StartEnvironment(Value);
  void EndEnvironment(Value);
};


class PhpHtmlImplementation : public Implementation, public Php::Base
{
private:
  std::map<std::string, Value> variables;
public:
  /// Interpreter functions
  std::shared_ptr<Environment> Global();
  std::shared_ptr<Environment> Create(Token name, std::shared_ptr<Environment> parent);
  std::string Escaped(TokenType escapetype, char escapee);
  std::string LineBreak() const;
  std::string Ampersand() const;

  /// TeX functions
  Php::Value setGlobalVar(Php::Parameters &parameters);
  Php::Value getGlobalVar(Php::Parameters &parameters);
  void EmitFatal(Php::Parameters &parameters);
  void EmitError(Php::Parameters &parameters);
  void EmitWarning(Php::Parameters &parameters);
};

class PhpHtmlModule
{
private:
  std::unordered_set<std::string> functions;
  std::string envname;

public:
  PhpHtmlModule(std::string phpname);
};

class MarTeX : public Php::Base
{
private:
  std::string last_result;
  std::string error_log;
  std::vector<std::string> modules;
  void process_errors(ErrorReporter &reporter);

public:
  MarTeX();
  void RegisterModule(Php::Parameters &params);
  void Parse(Php::Parameters &params);

  Php::Value HasError();
  Php::Value GetErrors();
  Php::Value GetResults();
};