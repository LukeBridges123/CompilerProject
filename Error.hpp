#pragma once
#include "Type.hpp"
#include "lexer.hpp"

using namespace emplex;

// From WordLang Error
template <typename... Ts>
[[noreturn]] void Error(size_t line_num, Ts... message) {
  std::cerr << "ERROR (line " << line_num << "): ";
  (std::cerr << ... << message);
  std::cerr << std::endl;
  exit(1);
}

template <typename... Ts>
[[noreturn]] void Error(Token const &token, Ts... message) {
  Error(token.line_id, message...);
}

template <typename... Ts>
[[noreturn]] void ErrorUnexpected(Token const &token, Ts... expected) {
  std::cerr << "ERROR (line " << token.line_id << "): ";
  std::cerr << "Unexpected token '" << token.lexeme << "'"
            << " of type " << Lexer::TokenName(token) << std::endl;
  // adding constexpr here to silence compiler warning (and check at compile
  // time!) from https://stackoverflow.com/a/46474191/4678913
  if constexpr (sizeof...(expected) > 0) {
    std::cerr << '\t' << "Expected token type(s): ";
    (std::cerr << ... << Lexer::TokenName(expected));
    std::cerr << std::endl;
  }
  exit(1);
}

template <typename... Ts> [[noreturn]] void ErrorNoLine(Ts... message) {
  std::cerr << "ERROR: ";
  (std::cerr << ... << message);
  std::cerr << std::endl;
  exit(1);
}

void ErrorUnsupportedUnary(Token const &token, Type const &type);
void ErrorUnsupportedBinary(Token const &token, Type const &lhs,
                            Type const &rhs);
