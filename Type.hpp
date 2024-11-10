#pragma once
#include "lexer.hpp"
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
  std::string TypeName() const;
};
