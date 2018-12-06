#pragma once

#include "language/value.hpp"
#include <phpcpp.h>
#include <vector>


Value PhpToCpp(Php::Value);
Php::Value CppToPhp(Value);
Php::Value CppToPhp(std::vector<Value>);