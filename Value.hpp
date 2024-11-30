#pragma once
#include "Type.hpp"

#include <stdexcept>
#include <type_traits>
#include <variant>
#include <string>

using ValueType = std::variant<int, double, char, size_t>;

class Value {
private:
  ValueType value;

public:
  size_t line_declared{};
  bool assigned = false;

  // Constructor for variable with the value known
  template <typename T>
  Value(size_t line, T val) : value(val), line_declared(line){};

  template <typename T> Value(T &&val) : value(std::forward<T>(val)){};

  Value(Value const &) = default;
  Value(Value &&) = default;

  // Returns the variant
  const ValueType getVariant() const { return value; };

  // Returns the literal value variant stored in the variant
  const ValueType getValue() const;

  // Returns the type of the variable
  VarType getType() const { return VarType(*this); }

  // Checks if the type of the value given is the same as the variant declared
  // and updates the value
  template <typename T> void UpdateValue(T val) {
    // Check if the types are the same
    bool is_same_type = std::visit(std::is_same_v<decltype(val), T>, value);

    if (!is_same_type) {
      // Todo: Give better error
      throw std::invalid_argument("Invalid assignment to a different Type!");
    }

    value = val;
    assigned = true;
  }


  std::string toString() const {
        return std::visit([](const auto& arg) -> std::string {
            if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, char>) {
              return std::string(1, arg); // Convert char to a string
            } else {
              return std::to_string(arg); // Use std::to_string for numbers
            }
        }, value);
    }
};
