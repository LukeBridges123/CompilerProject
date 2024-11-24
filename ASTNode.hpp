#pragma once

#include <cmath>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "State.hpp"
#include "Type.hpp"
#include "Value.hpp"
#include "WAT.hpp"

const std::unordered_map<std::string, std::string> LITERAL_TO_WAT = {
    {"+", "add"},   {"*", "mul"}, {"-", "sub"}, {"/", "div"},
    {"%", "rem_u"}, {"<", "lt"},  {">", "gt"},  {"<=", "le"},
    {">=", "ge"},   {"==", "eq"}, {"!=", "ne"},
};

class ASTNode {
public:
  enum Type {
    EMPTY = 0,
    MODULE,
    SCOPE,
    ASSIGN,
    IDENTIFIER,
    CONDITIONAL,
    OPERATION,
    LITERAL,
    CAST_CHAR,
    CAST_DOUBLE,
    CAST_INT,
    WHILE,
    FUNCTION,
    RETURN,
    CONTINUE,
    BREAK,
    FUNCTION_CALL
  };
  Type const type;
  std::optional<Value> value = std::nullopt;
  size_t var_id{};
  std::string literal{};

  // ASTNode copies are expensive, so only allow moves
  ASTNode(ASTNode &) = delete;
  ASTNode(ASTNode &&) = default;

  ASTNode(Type type = EMPTY) : type(type) {};
  ASTNode(Type type, std::string literal) : type(type), literal(literal) {};

  template <typename T>
  ASTNode(Type type, T val) : type(type), value(Value(val)){};
  ASTNode(Type type, size_t var_id) : type(type), var_id(var_id) {};

  template <typename... Ts>
  ASTNode(Type type, std::string literal, Ts &&...children)
      : type(type), literal(literal) {
    AddChildren(std::forward<Ts>(children)...);
  }

  operator int() const { return type; }

  void AddChild(ASTNode &&node) {
    if (node) {
      children.push_back(std::forward<ASTNode>(node));
    }
  }

  template <typename T, typename... Rest>
  void AddChildren(T node, Rest... rest) {
    AddChild(std::forward<T>(node));
    AddChildren(std::forward<Rest...>(rest...));
  }

  template <typename T> void AddChildren(T node) {
    AddChild(std::forward<T>(node));
  }

  bool hasValue() { return value != std::nullopt; }

  std::optional<VarType> getType() {
    if (!hasValue()) {
      return std::nullopt;
    }

    return value->getType();
  }

  WATExpr EmitModule(State &state) const;

  VarType ReturnType(SymbolTable const &table) const;
  bool HasReturn(State const &state) const;

private:
  std::vector<ASTNode> children{};

  std::vector<WATExpr> Emit(State &state) const;
  std::vector<WATExpr> EmitLiteral(State &state) const;
  std::vector<WATExpr> EmitScope(State &state) const;
  std::vector<WATExpr> EmitAssign(State &state) const;
  std::vector<WATExpr> EmitChainAssign(State &state) const;
  std::vector<WATExpr> EmitIdentifier(State &state) const;
  std::vector<WATExpr> EmitConditional(State &state) const;
  std::vector<WATExpr> EmitOperation(State &state) const;
  std::vector<WATExpr> EmitWhile(State &state) const;
  std::vector<WATExpr> EmitFunction(State &state) const;
  std::vector<WATExpr> EmitContinue(State &state) const;
  std::vector<WATExpr> EmitBreak(State &state) const;
};
