#include <iterator>
#include <stdexcept>

#include "Error.hpp"
#include "SymbolTable.hpp"

std::optional<size_t> SymbolTable::FindVarMaybe(std::string const &name) const {
  for (auto curr_scope = scope_stack.rbegin(); curr_scope != scope_stack.rend();
       curr_scope++) {
    auto result = curr_scope->find(name);
    if (result != curr_scope->end()) {
      return result->second;
    }
  }
  return std::nullopt;
}

void SymbolTable::PushScope() { this->scope_stack.emplace_back(); }

void SymbolTable::PopScope() {
  if (scope_stack.size() == 0) {
    throw std::runtime_error("tried to pop nonexistent scope");
  } else if (scope_stack.size() == 1) {
    throw std::runtime_error("tried to pop outermost scope");
  }
  scope_stack.pop_back();
}

size_t SymbolTable::FindVar(std::string const &name, size_t line_num) const {
  std::optional<size_t> result = FindVarMaybe(name);
  if (result) {
    return result.value();
  }
  Error(line_num, "Unknown variable ", name);
}

bool SymbolTable::HasVar(std::string const &name) const {
  return FindVarMaybe(name).has_value();
}

size_t SymbolTable::AddVar(std::string const &name, size_t line_num,
                           double value = 0.0) {
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

size_t SymbolTable::AddFunction(std::string const &name, size_t line_num) {
  size_t idx = this->functions.size();
  functions.emplace_back(name, line_num);
  return idx;
}

double SymbolTable::GetValue(size_t var_id, Token const *token) const {
  if (!variables[var_id].initialized) {
    if (token) {
      Error(*token, "attempt to access uninitialized variable ", token->lexeme);
    } else {
      ErrorNoLine("Attempt to access uninitialized variable");
    }
  }
  return variables[var_id].value;
}

void SymbolTable::SetValue(size_t var_id, double new_value) {
  variables[var_id].value = new_value;
  variables[var_id].initialized = true;
}
