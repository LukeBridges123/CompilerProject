#pragma once

#include <cmath>
#include <string>
#include <vector>

#include "State.hpp"
#include "WAT.hpp"

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
    WHILE,
    STRING,
    FUNCTION,
    RETURN // TODO: should return actually be a node?
  };
  Type const type;
  double value{};
  size_t var_id{};
  std::string literal{};
  Token const *token = nullptr; // for error reporting

  // ASTNode copies are expensive, so only allow moves
  ASTNode(ASTNode &) = delete;
  ASTNode(ASTNode &&) = default;

  ASTNode(Type type = EMPTY) : type(type) {};
  ASTNode(Type type, std::string literal) : type(type), literal(literal) {};
  ASTNode(Type type, double value) : type(type), value(value) {};
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

  template <typename T> void AddChildren(T node) {
    AddChild(std::forward<T>(node));
  }

  WATExpr EmitModule(State &state) const;
  std::vector<WATExpr> Emit(State &state) const;
  std::vector<WATExpr> EmitLiteral(State &state) const;
  std::vector<WATExpr> EmitScope(State &state) const;
  std::vector<WATExpr> EmitAssign(State &state) const;
  std::vector<WATExpr> EmitIdentifier(State &state) const;
  std::vector<WATExpr> EmitConditional(State &state) const;
  std::vector<WATExpr> EmitOperation(State &state) const;
  std::vector<WATExpr> EmitWhile(State &state) const;
  std::vector<WATExpr> EmitFunction(State &state) const;
};
