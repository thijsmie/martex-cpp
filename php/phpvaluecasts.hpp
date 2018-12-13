#pragma once

#include "language/value.hpp"
#include <phpcpp.h>
#include <vector>


Value PhpToCpp(Php::Value);
Php::Value CppToPhp(Value);
Php::Value CppToPhp(std::vector<Value>);

Php::Value html(Php::Parameters&);
Php::Value attr(Php::Parameters&);
Php::Value value(Php::Parameters&);
Php::Value batch(Php::Parameters&);
Php::Value ampersand(Php::Parameters&);
Php::Value newline(Php::Parameters&);