#include "Value.hpp"
#include "Error.hpp"

#include <type_traits>

template <typename T> Value::Value(std::string id_name, size_t line, T val) {
  value = val;
  assigned = true;
  name = id_name;
  line_declared = line;
}

Value::Value(std::string id_name, Type type, size_t line) {
  name = id_name;
  line_declared = line;

  switch (type.id) {
  case Type::INT:
    value = int();
    break;
  case Type::CHAR:
    value = char();
    break;
  case Type::DOUBLE:
    value = double();
    break;
  default:
    Error(line, "Declaration of value with unknown Type!");
    break;
  }
}

const std::optional<ValueType> Value::GetValue() const {
  if (!assigned) {
    // No value assigned
    return std::nullopt;
  }

  return value;
}

template <typename T> void Value::UpdateValue(T val) {
  // Check if the types are the same
  bool is_same_type = std::visit(std::is_same_v<decltype(val), T>, value);

  if (!is_same_type) {
    // Todo: Give better error
    throw std::invalid_argument("Invalid assignment to a different Type!");
  }

  value = val;
  assigned = true;
}
