#pragma once

#include <cassert>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "Type.hpp"

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
  size_t FindFunction(std::string const &name, size_t line_num) const;
  bool CheckTypes(size_t function_id, std::vector<VarType> arg_types,
                  size_t line_num) const;
};

struct State {
  SymbolTable table{};
  std::vector<size_t> loop_idx{};
  std::vector<std::string> string_literals{};
  size_t string_pos = 0;

  size_t AddString(std::string const &literal);
};
