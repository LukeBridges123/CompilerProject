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

Type::Type(Value &value){
  auto val = *(value.getValue());
  if (std::holds_alternative<int>(val)){ id = Type::INT; }
  else if (std::holds_alternative<double>(val)){ id = Type::DOUBLE; }
  else if (std::holds_alternative<char>(val)){ id = Type::CHAR; }
  else {
    Error(value.getDeclaredLine(), "Unknown type!");
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

std::string Type::WATType() const {
  switch (id) {
  case Type::INT:
  case Type::CHAR:
    return "i32";
  case Type::DOUBLE:
    return "f64";
  default:
    throw std::invalid_argument("Attempt to access unknown type");
  }
};
