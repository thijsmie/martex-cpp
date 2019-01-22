#pragma once

#include "token.hpp"

class UnknownError : public std::exception
{
    const char *what() const throw()
    {
        return message.c_str();
    }

  public:
    std::string message;

    UnknownError(std::string message) : message(message){};
};