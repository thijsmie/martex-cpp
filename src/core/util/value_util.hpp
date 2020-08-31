#pragma once

#include "core/language/value.hpp"
#include <vector>

namespace util
{
void ltrimws(std::vector<Value> &);
void rtrimws(std::vector<Value> &);
void trimws(std::vector<Value> &);
} // namespace util