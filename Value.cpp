#include "Value.hpp"
#include <stdexcept>
#include <variant>

const ValueType Value::getValue() const {
  if (std::holds_alternative<int>(value)) {
    return std::get<int>(value);
  } else if (std::holds_alternative<double>(value)) {
    return std::get<double>(value);
  } else if (std::holds_alternative<char>(value)) {
    return static_cast<int>(std::get<char>(value));
  } else if (std::holds_alternative<size_t>(value)) {
    return std::get<size_t>(value);
  }
  throw std::invalid_argument("Unknown value in variant");
}
