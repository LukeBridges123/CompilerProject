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
  std::string name{};
  size_t line_declared{};
  bool assigned = false;

  // Constructor for variable with the value known
  template <typename T> Value(std::string id_name, size_t line, T val);

  // Constructor for variable declaration without value
  Value(std::string id_name, Type type, size_t line);

  // Returns the variant so the value can be extracted
  const std::optional<ValueType> GetValue() const;

  // Returns the type of the variable
  Type GetType() const { return Type(*this); }

  // Checks if the type of the value given is the same as the variant declared
  // and updates the value
  template <typename T> void UpdateValue(T val);
};
