#include "Value.hpp"
#include "Error.hpp"

#include <type_traits>


// Value::Value(Type type, size_t line) {
//     line_declared = line;

//     switch (type.id) {
//     case Type::INT:
//         value = int();
//         break;
//     case Type::CHAR:
//         value = char();
//         break;
//     case Type::DOUBLE:
//         value = double();
//         break;
//     default:
//         Error(line, "Declareation of Unknown Type!");
//         break;
//     }
// }

const ValueType Value::getValue() const {
    if (std::holds_alternative<int>(value)){ return std::get<int>(value); }
    else if (std::holds_alternative<double>(value)){ return std::get<double>(value); }
    else { return std::get<char>(value); }
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

