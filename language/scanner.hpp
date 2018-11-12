#ifndef SCANNER_H_
#define SCANNER_H_

#include <string>
#include <vector>

#include "token.hpp"
#include "error_reporter.hpp"

class Scanner {
 public:
  Scanner(std::string source, ErrorReporter& error_reporter);
  std::vector<Token> ScanTokens();

 private:
  std::string source;
  ErrorReporter& error_reporter;
  std::vector<Token> tokens;

  int start = 0; // first character in current lexeme
  int current = 0; // current character
  int line = 1; 

  bool IsAtEnd();
  void AddToken(TokenType type, std::string lexeme);
  void AddToken(TokenType type);
  void EatWS();
  void DropWS();
  char Peek();
  char PeekNext();
  char Advance();
  bool Match(char c);
  void ScanCommandOrSpecial();
  void ScanToken();
  void Error(std::string error);
};


#endif  // SCANNER_H_