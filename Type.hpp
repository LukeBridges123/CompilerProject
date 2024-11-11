#pragma once
#include "lexer.hpp"
#include "Value.hpp"
#include <string>

using namespace emplex;

// TODO: integrate with actual variable values
class Type {
public:
  enum TypeId {
    UNKNOWN,
    CHAR,
    INT,
    DOUBLE,
  };

  TypeId id;

  Type(TypeId id) : id(id) {};
  Type(Token const &token);
  Type(Value &value);

  std::string TypeName() const;
  std::string WATType() const;
};
