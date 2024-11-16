#include "Type.hpp"
#include "Error.hpp"
#include "Value.hpp"

#include <stdexcept>

VarType::VarType(Token const &token) {
  std::string const &lexeme = token.lexeme;
  if (lexeme == "int") {
    id = VarType::INT;
  } else if (lexeme == "char") {
    id = VarType::CHAR;
  } else if (lexeme == "double") {
    id = VarType::DOUBLE;
  } else {
    Error(token, "Unknown type ", lexeme);
  }
}

VarType::VarType(Value const &value) {
  if (std::holds_alternative<int>(value.getVariant())) {
    id = VarType::INT;
  } else if (std::holds_alternative<double>(value.getVariant())) {
    id = VarType::DOUBLE;
  } else if (std::holds_alternative<char>(value.getVariant())) {
    id = VarType::CHAR;
  } else {
    Error(value.line_declared, "Unknown type!");
  }
}

std::string VarType::TypeName() const {
  switch (id) {
  case VarType::INT:
    return "int";
  case VarType::CHAR:
    return "char";
  case VarType::DOUBLE:
    return "double";
  default:
    throw std::invalid_argument("Attempt to access unknown type");
  }
}

std::string VarType::WATType() const {
  switch (id) {
  case VarType::INT:
  case VarType::CHAR:
    return "i32";
  case VarType::DOUBLE:
    return "f64";
  default:
    throw std::invalid_argument("Attempt to access unknown type");
  }
}

std::string VarType::WATOperation(std::string operation, bool is_signed) const {
  std::string inst = WATType() + "." + operation;
  if (is_signed && (id == INT || id == CHAR)) {
    inst += "_s";
  }
  return inst;
}
