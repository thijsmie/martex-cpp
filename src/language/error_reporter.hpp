#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include "token.hpp"

class ErrorReporter
{
  public:
    ErrorReporter();
    bool HadError();
    void Report(int line, std::string where, std::string message);
    void Error(int line, std::string message);
    void Error(Token token, std::string message);
    void ResetErrorFlag();
    void Log(std::string message);
    void Log(Token token, std::string);

    std::string Output();

  private:
    bool hadErrorFlag;
    std::stringstream myLog;
};