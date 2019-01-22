#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>

#include "core/language/environment.hpp"
#include "core/language/token.hpp"
#include "core/language/module.hpp"


class Implementation
{
  private:
    std::vector<std::shared_ptr<Module>> modules;
  public:
    Implementation(std::vector<std::shared_ptr<Module>>);
    
    std::shared_ptr<Environment> Create(Token name, std::shared_ptr<Environment> parent);
    
    std::string Escaped(TokenType escapetype, char escapee);
    std::string LineBreak() const;
    std::string Ampersand() const;
};
