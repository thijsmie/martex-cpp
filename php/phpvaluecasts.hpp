#pragma once

#include "language/value.hpp"
#include <phpcpp.h>
#include <vector>


Value PhpToCpp(const Php::Value&);
Php::Value CppToPhp(const Value&);
Php::Value CppToPhp(const std::vector<Value>&);
std::vector<Value> PhpToCppMulti(const unsigned char *b, int &pos, const uint32_t end);


Php::Value PhpAsBytes(Php::Parameters&);
Php::Value html(Php::Parameters&);
Php::Value attr(Php::Parameters&);
Php::Value mstr(Php::Parameters&);
Php::Value ampersand(Php::Parameters&);
Php::Value newline(Php::Parameters&);