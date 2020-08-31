#pragma once

#include "core/util/cppmodule.hpp"
#include "core/language/module.hpp"

#include <vector>
#include <map>
#include <string>
#include <algorithm>

/*
Class allowing frontend wasm implementation to polyfill commands
that are implemented fully in backend. Don't run on arbitrary data.
Declare \html and \attr in newcommand to block access.
*/
class PolyfillLib : public util::CppModule<PolyfillLib>
{
  public:
    PolyfillLib();

    std::vector<std::string> GetEnvs();
    std::shared_ptr<Environment> MakeEnv(std::string, std::shared_ptr<Environment>);

    Value attr(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value html(std::shared_ptr<Environment>, Token, std::vector<Value>);
};
