#pragma once

#include "language/value.hpp"
#include <phpcpp.h>
#include <vector>


Value PhpToCpp(const Php::Value&);
Php::Value CppToPhp(const Value&);
Php::Value CppToPhp(const std::vector<Value>&);

Php::Value html(Php::Parameters&);
Php::Value attr(Php::Parameters&);
Php::Value value(Php::Parameters&);
Php::Value batch(Php::Parameters&);
Php::Value ampersand(Php::Parameters&);
Php::Value newline(Php::Parameters&);