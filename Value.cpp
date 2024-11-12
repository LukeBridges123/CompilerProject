#include "Value.hpp"
#include "Error.hpp"

#include <type_traits>

template <typename T>
Value::Value(std::string idName, size_t line, T val) {
    value = val;
    assigned = true;
    name = idName;
    line_declared = line;
}

Value::Value(std::string idName, Type type, size_t line) {
    name = idName;
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
        // Add some error handling
        break;
    }
}

const std::optional<ValueType> Value::getValue() const {   
    if (!assigned) {
        // No value assigned
        return std::nullopt;
    }

    return value; 
}

template <typename T>
void Value::updateValue(T val) {
    bool isSameType = std::visit([&](const auto &val) {
            // Check if the types are the same
            return std::is_same_v<decltype(val), T>; }, value);
    
    if (!isSameType) {
        // Todo: Give better error
        throw std::invalid_argument("Invalid assignment to a different Type!");
    }

    value = val;
    assigned = true;
}