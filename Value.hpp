#pragma once
#include "lexer.hpp"
#include "Type.hpp"

#include <variant>
#include <string>

//using ValueType = std::variant<int, double, char>;

class Value {
private:
    std::variant<int, double, char> value;
    std::string name{};
    size_t line_declared{};
    bool assigned = false;
public:
    // Constructor for variable with the value known
    template <typename T>
    Value(std::string idName, size_t line, T val);

    // Constructor for variable declaration without value
    Value(std::string idName, Type type, size_t line);
    
    // Returns the variant so the value can be extracted
    const std::variant<int, double, char> *getValue() const ;

    // Returns the line that the variable was declared at
    size_t getDeclaredLine() const { return line_declared; }

    // Returns the name of the variable
    std::string getName() const { return name; }

    // Returns the type of the variable
    Type getType() const { return Type(*this); }

    // Checks if the type of the value given is the same as the variant declared and updates the value
    template <typename T>
    void updateValue(T val);
};
