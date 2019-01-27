#pragma once

#include <string>
#include "token.hpp"

class Implementation
{
  public:
    static std::string Escaped(TokenType escapetype, char escapee);
    static std::string LineBreak();
    static std::string Ampersand();
};
