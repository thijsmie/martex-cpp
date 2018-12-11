#include "error_reporter.hpp"

ErrorReporter::ErrorReporter() : hadErrorFlag(false), myLog() {}

bool ErrorReporter::HadError()
{
    return hadErrorFlag;
}

void ErrorReporter::Report(int line, std::string where, std::string message)
{
    hadErrorFlag = true;
    myLog << "Error on line " << std::setw(2) << line << " at '" << where << "', " << message << "." << std::endl;
}

void ErrorReporter::Error(int line, std::string message)
{
    hadErrorFlag = true;
    myLog << "Error on line " << std::setw(2) << line << ", " << message << "." << std::endl;
}

void ErrorReporter::Error(Token token, std::string message)
{
    hadErrorFlag = true;
    Report(token.GetLine(), token.GetLexeme(), message);
}

void ErrorReporter::Log(std::string message)
{
    myLog << message << std::endl;
}

void ErrorReporter::Log(Token token, std::string message)
{
    myLog << "FYI at '" << token.GetLexeme() << "' on line " << std::setw(2) << token.GetLine() << ",  " << message << "." << std::endl;
}

void ErrorReporter::ResetErrorFlag()
{
    hadErrorFlag = false;
    myLog.clear();
}


std::string ErrorReporter::Output()
{
    return myLog.str();
}
