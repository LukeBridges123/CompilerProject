#pragma once

#include <cassert>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "Type.hpp"
#include "Value.hpp"

using scope_t = std::unordered_map<std::string, size_t>;

struct VariableInfo {
  std::string name{};
  size_t line_declared{};
  VarType type_var;
  bool is_assigned{};
};

struct FunctionInfo {
  std::string name;
  size_t line_declared{};
  // first `parameters` variables in `variables` are parameters
  size_t parameters = 0;
  // index of variables used in function
  std::vector<size_t> variables{};
  VarType rettype = VarType::UNKNOWN;
};

class SymbolTable {

private:
  std::vector<scope_t> scope_stack{1};

  std::optional<size_t> FindVarMaybe(std::string const &name) const;

public:
  std::vector<VariableInfo> variables{};
  std::vector<FunctionInfo> functions{};

  void PushScope();
  scope_t PopScope();
  size_t FindVar(std::string const &name, size_t line_num) const;
  bool HasVar(std::string const &name) const;
  size_t AddVar(std::string const &name, VarType type, size_t line_num);
  size_t AddFunction(std::string const &name, size_t line_num);
};

struct State {
  SymbolTable const table;
  std::vector<size_t> loop_idx = {0};
};
