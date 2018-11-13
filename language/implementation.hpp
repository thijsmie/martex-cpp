#ifndef ESCAPED_H
#define ESCAPED_H

#include <string>
#include <memory>
#include <unordered_map>
#include "environment.h"
#include "token.h"

class Implementation
{
  public:
    virtual std::shared_ptr<Environment> Global() = 0;
    virtual std::shared_ptr<Environment> Create(Token name, std::shared_ptr<Environment> parent) = 0;
    virtual std::string Escaped(TokenType escapetype, char escapee) = 0;
    virtual std::string LineBreak() const = 0;
    virtual std::string Ampersand() const = 0;
};

#endif // ESCAPED_Hs