#pragma once

#include <string>
#include "token.hpp"

class ErrorReporter
{
  public:
    ErrorReporter();
    bool HadError();
    bool IsFatal();
    void Report(int line, std::string where, std::string message);
    void Error(int line, std::string message);
    void Error(Token token, std::string message);
    void ResetErrorFlag();

    std::string Output();

  private:
    bool hadErrorFlag;
    bool hadFatalError;
};