#pragma once

#include <cmath>
#include <string>
#include <vector>
#include <optional>

#include "SymbolTable.hpp"
#include "WAT.hpp"
#include "Value.hpp"
#include "Type.hpp"

class ASTNode {

private:
  std::vector<ASTNode> children{};

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
    CAST_INT,
    CAST_DOUBLE,
    WHILE,
    CAST_CHAR,
    FUNCTION,
    RETURN // TODO: should return actually be a node?
  };
  Type const type;
  //double value{};
  std::optional<Value> value = std::nullopt;
  size_t var_id{};
  std::string literal{};
  Token const *token = nullptr; // for error reporting

  // ASTNode copies are expensive, so only allow moves
  ASTNode(ASTNode &) = delete;
  ASTNode(ASTNode &&) = default;

  ASTNode(Type type = EMPTY) : type(type) {};
  ASTNode(Type type, std::string literal) : type(type), literal(literal) {};

  template <typename T>
  ASTNode(Type type, T val) : type(type), value(Value(val)) {};
  // ASTNode(Type type, char val);
  ASTNode(Type type, size_t var_id, Token const *token)
      : type(type), var_id(var_id), token(token) {};

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

  template <typename T> 
  void AddChildren(T node) {
    AddChild(std::forward<T>(node));
  }

  bool hasValue() { return value != std::nullopt; }

  std::optional<VarType> getType() {
    if (!hasValue()) {
      return std::nullopt;
    }

    return value->getType();
  }

  WATExpr EmitModule(SymbolTable const &symbols) const;
  std::vector<WATExpr> Emit(SymbolTable const &symbols) const;
  std::vector<WATExpr> EmitLiteral(SymbolTable const &symbols) const;
  std::vector<WATExpr> EmitScope(SymbolTable const &symbols) const;
  std::vector<WATExpr> EmitAssign(SymbolTable const &symbols) const;
  std::vector<WATExpr> EmitIdentifier(SymbolTable const &symbols) const;
  std::vector<WATExpr> EmitConditional(SymbolTable const &symbols) const;
  std::vector<WATExpr> EmitOperation(SymbolTable const &symbols) const;
  std::vector<WATExpr> EmitWhile(SymbolTable const &symbols) const;
  std::vector<WATExpr> EmitFunction(SymbolTable const &symbols) const;
};
