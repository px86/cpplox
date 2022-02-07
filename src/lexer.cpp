#include "lexer.hpp"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <iostream>
#include <string>

auto Token::to_string() const -> std::string {
  std::string type_str = [this]() { // Immediately invoked lambda
    switch (type) {
    case  TokenType::LEFT_PAREN: return "LEFT_PAREN";
    case  TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
    case  TokenType::LEFT_BRACE: return "LEFT_BRACE";
    case  TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
    case  TokenType::COMMA: return "COMMA";
    case  TokenType::DOT: return "DOT";
    case  TokenType::MINUS: return "MINUS";
    case  TokenType::PLUS: return "PLUS";
    case  TokenType::SEMICOLON: return "SEMICOLON";
    case  TokenType::SLASH: return "SLASH";
    case  TokenType::STAR: return "STAR";
    case  TokenType::BANG: return "BANG";
    case  TokenType::BANG_EQUAL: return "BANG_EQUAL";
    case  TokenType::EQUAL: return "EQUAL";
    case  TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
    case  TokenType::GREATER: return "GREATER";
    case  TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
    case  TokenType::LESS: return "LESS";
    case  TokenType::LESS_EQUAL: return "LESS_EQUAL";
    case  TokenType::IDENTIFIER: return "IDENTIFIER";
    case  TokenType::STRING: return "STRING";
    case  TokenType::NUMBER: return "NUMBER";
    case  TokenType::AND: return "AND";
    case  TokenType::CLASS: return "CLASS";
    case  TokenType::ELSE: return "ELSE";
    case  TokenType::FALSE: return "FALSE";
    case  TokenType::FUN: return "FUN";
    case  TokenType::FOR: return "FOR";
    case  TokenType::IF: return "IF";
    case  TokenType::NIL: return "NIL";
    case  TokenType::OR: return "OR";
    case  TokenType::PRINT: return "PRINT";
    case  TokenType::RETURN: return "RETURN";
    case  TokenType::SUPER: return "SUPER";
    case  TokenType::THIS: return "THIS";
    case  TokenType::TRUE: return "TRUE";
    case  TokenType::VAR: return "VAR";
    case  TokenType::WHILE: return "WHILE";
    case  TokenType::LOX_EOF: return "LOX_EOF";
    };
    return "IF YOU SEE THIS, THERE IS A BUG!"; // control should not reach here
  }();
  auto token_str = type_str + " [" + std::to_string(start) + ", "
    + std::to_string(len) + ']';

  return token_str;
}

// offset=0 means return the char at current position.
inline char Lexer::peek(int offset=0) const {
  int i = m_current + offset;
  if (i < (int)m_source.size() && i >= 0) {
    return m_source.at(i);
  } else return '\0';
}

// offset=0 means consume current character, and move forward.
inline char Lexer::consume(int offset=0) {
  int i = m_current + offset;
  if (i < (int)m_source.size() && i >= 0) {
    m_current = i + 1;
    return m_source.at(i);
  } else return '\0';
}

inline void Lexer::emit_token(TokenType type, int len=1) {
  Token token(type, m_current, len);
  m_tokens.push_back(std::move(token));
  consume(len-1);
}

TokenType resolve_identifier(std::string s) {
  if (!s.compare("if")) return TokenType::IF;
  if (!s.compare("else")) return TokenType::ELSE;
  if (!s.compare("and")) return TokenType::AND;
  if (!s.compare("or")) return TokenType::OR;
  if (!s.compare("true")) return TokenType::TRUE;
  if (!s.compare("false")) return TokenType::FALSE;
  if (!s.compare("for")) return TokenType::FOR;
  if (!s.compare("while")) return TokenType::SUPER;
  if (!s.compare("class")) return TokenType::CLASS;
  if (!s.compare("fun")) return TokenType::FUN;
  if (!s.compare("return")) return TokenType::RETURN;
  if (!s.compare("NIL")) return TokenType::NIL;
  if (!s.compare("print")) return TokenType::PRINT;
  if (!s.compare("super")) return TokenType::SUPER;
  if (!s.compare("var")) return TokenType::VAR;
  if (!s.compare("this")) return TokenType::THIS;
  return TokenType::IDENTIFIER;
}

auto Lexer::lex() -> std::vector<Token>& {
  char c;
  while ((c = peek())) {
    switch (c) {
    case '\n':
    case '\t':
    case '\r':
    case ' ' : consume(); break;

    case '{': emit_token(TokenType::LEFT_BRACE); break;
    case '}': emit_token(TokenType::RIGHT_BRACE); break;
    case '(': emit_token(TokenType::LEFT_PAREN); break;
    case ')': emit_token(TokenType::RIGHT_PAREN); break;
    case ',': emit_token(TokenType::COMMA); break;
    case '.': emit_token(TokenType::DOT); break;
    case '+': emit_token(TokenType::PLUS); break;
    case '-': emit_token(TokenType::MINUS); break;
    case '*': emit_token(TokenType::STAR); break;
    case ';': emit_token(TokenType::SEMICOLON); break;

    case '/':
      { // SLASH or comment
	if (peek(1) == '/') consume(2);
	else emit_token(TokenType::SLASH);
      } break;
    case '!':
      { // BANG or BANG_EQUAL
	if (peek(1) == '=') emit_token(TokenType::BANG_EQUAL, 2);
	else emit_token(TokenType::BANG);
      } break;
    case '=':
      { // EQUAL or EQUAL_EQUAL
	if (peek(1) == '=') emit_token(TokenType::EQUAL_EQUAL, 2);
	else emit_token(TokenType::EQUAL);
      } break;
    case '>':
      { // GREATER_EQUAL or GREATER
	if (peek(1) == '=') emit_token(TokenType::GREATER_EQUAL, 2);
	else emit_token(TokenType::GREATER);
      } break;
    case '<':
      { // LESS_EQUAL or LESS
	if (peek(1) == '=') emit_token(TokenType::LESS_EQUAL, 2);
	else emit_token(TokenType::LESS);
      } break;
    case '"':
      { // String
	auto token = Token(TokenType::STRING, m_current, 0);
	consume();
	while (true) {
	  char t = peek();
	  if (!peek()) error("EOF reached while parsing a string", m_current);
	  if (t != '"') consume();
	  else if (peek(-1) != '\\') break;
	  else consume();
	}
	token.len = m_current - token.start;
	consume();
	m_tokens.push_back(std::move(token));
      }	break;

    default:
      { // NUMBER
	if (isdigit(c)) {
	  auto token = Token(TokenType::NUMBER, m_current, 0);
	  consume();
	  for (char t=peek(); t && isdigit(t); t=peek()) consume();
	  if (isalpha(peek())) error("Unexpected character", m_current);
	  token.len = m_current - token.start;
	  m_tokens.push_back(std::move(token));
	}
	// IDENTIFIER and keywords
	else if (isalpha(c) or c == '_') {
	  Token token; token.start = m_current;
	  consume();
	  for (char t=peek(); t && (isalnum(t) || t=='_'); t=peek()) consume();
	  token.len = m_current - token.start;
	  token.type = resolve_identifier(m_source.substr(token.start, token.len));
          m_tokens.push_back(std::move(token));
	}
	else error("unknown token", m_current);
      }
    }
  }

  m_tokens.push_back({TokenType::LOX_EOF, m_current, 0});
  return m_tokens;
}

// TODO: Move this function somewhere else.
void error(const char* desc, size_t position) {
  fprintf(stderr, "Error: %s , position %zu\n", desc, position);
  exit(1);
}
