#include "Type.hpp"
#include "Error.hpp"
#include "Value.hpp"

#include <stdexcept>

VarType::TypeId VarType::TypeFromValue(const Value &value) {
  const ValueType variant = value.getVariant();
  if (std::holds_alternative<int>(variant)) {
    return VarType::INT;
  } else if (std::holds_alternative<double>(variant)) {
    return VarType::DOUBLE;
  } else if (std::holds_alternative<char>(variant)) {
    return VarType::CHAR;
  } else if (std::holds_alternative<size_t>(variant)) {
    return VarType::STRING;
  }
  throw std::invalid_argument("Unknown value in variant");
}

VarType::TypeId VarType::TypeFromToken(const Token &token) {
  std::string const &lexeme = token.lexeme;
  if (lexeme == "int") {
    return VarType::INT;
  } else if (lexeme == "char") {
    return VarType::CHAR;
  } else if (lexeme == "double") {
    return VarType::DOUBLE;
  } else if (lexeme == "string") {
    return VarType::STRING;
  } else {
    Error(token, "Unknown type ", lexeme);
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
  case VarType::STRING:
    return "string";
  default:
    throw std::invalid_argument("Attempt to access unknown type");
  }
}

std::string VarType::WATType() const {
  switch (id) {
  case VarType::INT:
  case VarType::CHAR:
  case VarType::STRING:
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
