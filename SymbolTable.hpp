#pragma once

#include <cassert>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "Type.hpp"
#include "Value.hpp"

struct VariableInfo {
  std::string name{};
  size_t line_declared{};
  Type type = Type::DOUBLE;
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

  std::optional<size_t> FindVarMaybe(std::string const &name) const;

public:
  std::vector<Value> variables{};
  std::vector<FunctionInfo> functions{};

  void PushScope();
  void PopScope();
  size_t FindVar(std::string const &name, size_t line_num) const;
  bool HasVar(std::string const &name) const;
  size_t AddVar(std::string const &name, Type type, size_t line_num);
  size_t AddFunction(std::string const &name, size_t line_num);
};
