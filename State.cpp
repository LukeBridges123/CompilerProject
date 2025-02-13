#include <format>
#include <iterator>
#include <stdexcept>

#include "Error.hpp"
#include "State.hpp"

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

scope_t SymbolTable::PopScope() {
  if (scope_stack.size() == 0) {
    throw std::runtime_error("tried to pop nonexistent scope");
  } else if (scope_stack.size() == 1) {
    throw std::runtime_error("tried to pop outermost scope");
  }

  scope_t back = scope_stack.back();
  scope_stack.pop_back();
  return back;
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

size_t SymbolTable::AddVar(std::string const &name, VarType type,
                           size_t line_num) {
  // a function must be created before we can add variables
  assert(functions.size() > 0);

  auto curr_scope = scope_stack.rbegin();
  if (curr_scope->find(name) != curr_scope->end()) {
    Error(line_num, "Redeclaration of variable ", name);
  }
  // Some way to know if the variable as been assigned
  VariableInfo new_var_info = VariableInfo{name, line_num, type};
  size_t new_index = this->variables.size();
  variables.push_back(new_var_info);
  functions.back().variables.push_back(new_index);
  curr_scope->insert({name, new_index});
  return new_index;
}

size_t SymbolTable::AddFunction(std::string const &name, size_t line_num) {
  size_t idx = this->functions.size();
  functions.emplace_back(name, line_num);
  return idx;
}

size_t SymbolTable::FindFunction(std::string const &name,
                                 size_t line_num) const {
  for (size_t curr_function = 0; curr_function < functions.size();
       curr_function++) {
    if (functions.at(curr_function).name == name) {
      return curr_function;
    }
  }
  Error(line_num, "Unknown function ", name);
}

bool SymbolTable::CheckTypes(size_t function_id, std::vector<VarType> arg_types,
                             size_t line_num) const {
  assert(function_id < functions.size());

  if (functions.at(function_id).parameters != arg_types.size()) {
    Error(line_num,
          std::format("Called function with {} arguments, expected {}",
                      arg_types.size(), functions.at(function_id).parameters));
  }

  for (size_t i = 0; i < arg_types.size(); i++) {
    size_t var_id = functions.at(function_id).variables.at(i);

    if (variables.at(var_id).type_var != arg_types.at(i)) {
      return false;
    }
  }
  return true;
}

size_t State::AddString(std::string const &literal) {
  size_t pos = string_pos;
  string_pos += literal.size() + 1;
  string_literals.push_back(literal);
  return pos;
}
