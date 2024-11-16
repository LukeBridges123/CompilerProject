#include "Value.hpp"

const ValueType Value::getValue() const {
  if (std::holds_alternative<int>(value)) {
    return std::get<int>(value);
  } else if (std::holds_alternative<double>(value)) {
    return std::get<double>(value);
  } else {
    return (int)std::get<char>(value);
  }
}
