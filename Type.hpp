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
    NONE,
    CHAR,
    INT,
    DOUBLE,
  };

  TypeId id;

  VarType(TypeId id) : id(id) {};
  VarType(Token const &token);
  VarType(Value const &value);

  operator TypeId() const { return id; }

  std::string TypeName() const;
  std::string WATType() const;
  std::string WATOperation(std::string operation, bool is_signed = false) const;
};
