#pragma once
#include "Type.hpp"

#include <optional>
#include <string>
#include <variant>

using ValueType = std::variant<int, double, char>;

class Value {
private:
  ValueType value;

public:
  size_t line_declared{};
  bool assigned = false;

  // Constructor for variable with the value known
  template <typename T>
  Value(size_t line, T val) : value(val), line_declared(line){};

  template <typename T> Value(T val) : value(val){};

  // Constructor for variable declaration without value
  // Value(Type type, size_t line);

  // Returns the variant
  const ValueType getVariant() const { return value; };

  // Returns the literal value variant stored in the variant
  const ValueType getValue() const;

  // Returns the type of the variable
  VarType getType() const { return VarType(*this); }

  // Checks if the type of the value given is the same as the variant declared
  // and updates the value
  template <typename T> void UpdateValue(T val);
};
