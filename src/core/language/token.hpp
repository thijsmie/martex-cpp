#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>

enum TokenType {
  LEFT_BRACKET, RIGHT_BRACKET, LEFT_BRACE, RIGHT_BRACE,
  
  // Actionables
  QUOT, DUQUOT, TICK, HAT, TILT, DOT, DASH,
  ESCAPE, VAR,

  // Literals
  WORD, WHITESPACE, LINE,
  AMPERSAND, NEWLINE, 

  // Specials
  BEGIN_ENV, END_ENV, COMMAND, NEWCOMMAND,

  EOTF
};

class Token {
 public:
  Token(TokenType type, std::string lexeme, int line);
  std::string ToString() const;
  std::string GetLexeme() const;
  int GetLine() const;
  TokenType GetType() const;
 private:
  TokenType type;
  std::string lexeme;
  int line;

};

#endif  // TOKEN_H_