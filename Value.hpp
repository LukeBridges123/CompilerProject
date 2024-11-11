#pragma once
#include "lexer.hpp"
#include <variant>
#include <string>

using ValueType = std::variant<int, double, char>;

// TODO: integrate with actual variable values
class Value {
private:
    ValueType value;
    std::string name{};
    size_t line_declared{};
public:

    

};