#pragma once
#include "lexer.hpp"
#include <string>

using namespace emplex;

class Value;

class VarType {
public:
  enum TypeId {
    UNKNOWN,
    NONE,
    CHAR,
    INT,
    DOUBLE,
    STRING
  };

private:
  static TypeId TypeFromValue(Value const &value);
  static TypeId TypeFromToken(Token const &token);

public:
  TypeId id;

  VarType(TypeId id) : id(id) {};
  VarType(Token const &token) : id(TypeFromToken(token)) {};
  VarType(Value const &value) : id(TypeFromValue(value)) {};

  operator TypeId() const { return id; }

  std::string TypeName() const;
  std::string WATType() const;
  std::string WATOperation(std::string operation, bool is_signed = false) const;
};
