#pragma once
#include "lexer.hpp"
#include "Type.hpp"

#include <variant>
#include <optional>
#include <string>

using ValueType = std::variant<int, double, char>;

class Value {
private:
    ValueType value;
public:
    std::string name{};
    size_t line_declared{};
    bool assigned = false;

    // Constructor for variable with the value known
    template <typename T>
    Value(std::string idName, size_t line, T val);

    // Constructor for variable declaration without value
    Value(std::string idName, Type type, size_t line);
    
    // Returns the variant so the value can be extracted
    const std::optional<ValueType> getValue() const ;

    // Returns the type of the variable
    Type getType() const { return Type(*this); }

    // Checks if the type of the value given is the same as the variant declared and updates the value
    template <typename T>
    void updateValue(T val);
};
