#pragma once
#include "lexer.hpp"
#include <string>

using namespace emplex;

class Value;

// TODO: integrate with actual variable values
class VarType {
public:
  enum TypeId {
    UNKNOWN,
    CHAR,
    INT,
    DOUBLE,
  };

  TypeId id;

  VarType(TypeId id) : id(id) {};
  VarType(Token const &token);
  VarType(Value const &value);

  std::string TypeName() const;
  std::string WATType() const;
};
