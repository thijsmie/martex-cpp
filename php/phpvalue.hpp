#pragma once

#include "language/value.hpp"

#include <phpcpp.h>

#include <vector>
#include <map>
#include <string>


class PhpValue : public Php::Base
{
  private:
    bool php_constructed;
    Value v;
  public:
    PhpValue();
    PhpValue(Value);
    PhpValue(std::string);

    void __construct(Php::Parameters&);
    std::string __toString();
    Php::Value __get(const Php::Value&);
    void __set(const Php::Value&, const Php::Value&);

    Value val();
};

class PhpArray : public Php::Base, public Php::Countable, public Php::ArrayAccess
{
  private:
    bool php_constructed;
    std::string tag;
    Php::Array content;
  public:
    PhpArray();
    PhpArray(Value);
    PhpArray(std::vector<Value>);

    void setVals(std::vector<Php::Value>);
    void __construct(Php::Parameters&);
    std::string __toString();
    Php::Value __get(const Php::Value&);
    void __set(const Php::Value&, const Php::Value&);

    // Array:
    long count() override;
    bool offsetExists(const Php::Value &key) override;
    void offsetSet(const Php::Value &key, const Php::Value &value) override;
    Php::Value offsetGet(const Php::Value &key) override;
    void offsetUnset(const Php::Value &key) override;

    Value val();
};


class PhpHtml : public Php::Base
{
  private:
    bool php_constructed;
    std::string tag;
    PhpArray *content;
    Php::Object arrayobj;
  public:
    PhpHtml();
    PhpHtml(Value);
    PhpHtml(std::string);
    PhpHtml(std::string, std::vector<Php::Value>);

    void __construct(Php::Parameters&);
    std::string __toString();
    Php::Value __get(const Php::Value&);
    void __set(const Php::Value&, const Php::Value&);

    Value val();
};


Php::Value cast_to_php(Value);

// Ease funcs
Php::Value html(Php::Parameters&);
Php::Value value(Php::Parameters&);
Php::Value batch(Php::Parameters&);
Php::Value ampersand(Php::Parameters&);
Php::Value newline(Php::Parameters&);