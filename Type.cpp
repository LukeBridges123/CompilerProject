#include "Type.hpp"
#include "Error.hpp"
#include <stdexcept>

Type::Type(Token const &token) {
  std::string const &lexeme = token.lexeme;
  if (lexeme == "int") {
    id = Type::INT;
  } else if (lexeme == "char") {
    id = Type::CHAR;
  } else if (lexeme == "double") {
    id = Type::DOUBLE;
  } else {
    Error(token, "Unknown type ", lexeme);
  }
}

std::string Type::TypeName() const {
  switch (id) {
  case Type::INT:
    return "int";
  case Type::CHAR:
    return "char";
  case Type::DOUBLE:
    return "double";
  default:
    throw std::invalid_argument("Attempt to access unknown type");
  }
};
