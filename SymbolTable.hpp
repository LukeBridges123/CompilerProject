#pragma once

// TODO(rose): accidentally git merged SymbolTable back into single header file

#include <cassert>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "Error.hpp"
#include "Type.hpp"

struct VariableInfo {
  std::string name{};
  double value{};
  size_t line_declared{};
  bool initialized = false;
};

// TODO: change string types eg. enum
struct FunctionInfo {
  std::string name;
  size_t line_declared{};
  std::vector<std::pair<std::string, size_t>> arguments{};
  Type rettype = Type::UNKNOWN;
};

class SymbolTable {
private:
  typedef std::unordered_map<std::string, size_t> scope_t;
  std::vector<scope_t> scope_stack{1};
  std::vector<VariableInfo> variables{};

  std::optional<size_t> FindVarMaybe(std::string const &name) const {
    for (auto curr_scope = scope_stack.rbegin();
         curr_scope != scope_stack.rend(); curr_scope++) {
      auto result = curr_scope->find(name);
      if (result != curr_scope->end()) {
        return result->second;
      }
    }
    return std::nullopt;
  }

public:
  std::vector<FunctionInfo> functions{};

  void PushScope() { this->scope_stack.emplace_back(); }

  void PopScope() {
    if (scope_stack.size() == 0) {
      throw std::runtime_error("tried to pop nonexistent scope");
    } else if (scope_stack.size() == 1) {
      throw std::runtime_error("tried to pop outermost scope");
    }
    scope_stack.pop_back();
  }

  size_t FindVar(std::string const &name, size_t line_num) const {
    std::optional<size_t> result = FindVarMaybe(name);
    if (result) {
      return result.value();
    }
    Error(line_num, "Unknown variable ", name);
  }

  bool HasVar(std::string const &name) const {
    return FindVarMaybe(name).has_value();
  }

  size_t AddVar(std::string const &name, size_t line_num, double value = 0.0) {
    auto curr_scope = scope_stack.rbegin();
    if (curr_scope->find(name) != curr_scope->end()) {
      Error(line_num, "Redeclaration of variable ", name);
    }
    VariableInfo new_var_info = VariableInfo{name, value, line_num};
    size_t new_index = this->variables.size();
    variables.push_back(new_var_info);
    curr_scope->insert({name, new_index});
    return new_index;
  }

  size_t AddFunction(std::string const &name, size_t line_num) {
    size_t idx = this->functions.size();
    functions.emplace_back(name, line_num);
    return idx;
  }

  double GetValue(size_t var_id, Token const *token) const {
    if (!variables[var_id].initialized) {
      if (token) {
        Error(*token, "attempt to access uninitialized variable ",
              token->lexeme);
      } else {
        ErrorNoLine("Attempt to access uninitialized variable");
      }
    }
    return variables[var_id].value;
  }

  void SetValue(size_t var_id, double new_value) {
    variables[var_id].value = new_value;
    variables[var_id].initialized = true;
  }
};
