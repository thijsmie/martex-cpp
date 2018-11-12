#include "token.hpp"

using std::string;

string tokenTypes[] = {
  // Single-character tokens.
  "LEFT_BRACKET", "RIGHT_BRACKET", "LEFT_BRACE", "RIGHT_BRACE",
  
  
  // Character mods
  "QUOT", "DUQUOT", "TICK", "HAT", "TILT", "DOT", "DASH",

  // Literals.
  "WORD", "WHITESPACE",
  "AMPERSAND", "NEWLINE", "LESS", "GREATER", 

  // Specials
  "BEGIN_ENV", "END_ENV", "COMMAND", 

  "EOTF"
};

Token::Token(TokenType type, string lexeme, int line) :
  type(type), lexeme(lexeme), line(line) {}

string Token::ToString() const {
  return tokenTypes[type] + " " + lexeme;
}

string Token::GetLexeme() const {
  return lexeme;
}

int Token::GetLine() const {
  return line;
}

TokenType Token::GetType() const {
  return type;
}