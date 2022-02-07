#pragma once

#include <cstddef>
#include <string>
#include <vector>

// Forward declare stuff.
enum class TokenType;
struct Token;
class Lexer;

struct Token {
  TokenType type;
  size_t start = -1;
  size_t len = 0;
  Token() = default;
  Token(TokenType type, size_t start, size_t len)
    :type(type), start(start), len(len) {}
  std::string to_string() const;
};

class Lexer {
public:
  Lexer(std::string &source)
    : m_source(source), m_current(0) {}
  std::vector<Token>& lex();

private:
  const std::string& m_source;
  size_t m_current;
  std::vector<Token> m_tokens;
  char peek(int offset) const;
  char consume(int offset);
  void emit_token(TokenType, int len);
};

enum class TokenType {
  // Single-character tokens.
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
  // One or two character tokens.
  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,
  // Literals.
  IDENTIFIER, STRING, NUMBER,
  // Keywords.
  AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
  PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

  LOX_EOF // because EOF seems to be already taken.

};

void error(const char* desc, size_t position);
TokenType resolve_identifier(std::string substr);
