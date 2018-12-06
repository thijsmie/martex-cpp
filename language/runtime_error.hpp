#pragma once

#include "token.hpp"

class RuntimeError : public std::exception
{
    const char *what() const throw()
    {
        std::string msg = std::to_string(token.GetLine()) + ": " + message;
        return msg.c_str();
    }

  public:
    Token token;
    std::string message;

    RuntimeError(Token token, std::string message) : token(token), message(message){};
};