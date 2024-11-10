#pragma once

#include <cmath>
#include <string>
#include <vector>

#include "SymbolTable.hpp"
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

  WATExpr EmitModule(SymbolTable const &symbols);
  std::vector<WATExpr> Emit(SymbolTable const &symbols);
  std::vector<WATExpr> EmitLiteral(SymbolTable const &symbols);
  std::vector<WATExpr> EmitScope(SymbolTable const &symbols);
  std::vector<WATExpr> EmitAssign(SymbolTable const &symbols);
  std::vector<WATExpr> EmitIdentifier(SymbolTable const &symbols);
  std::vector<WATExpr> EmitConditional(SymbolTable const &symbols);
  std::vector<WATExpr> EmitOperation(SymbolTable const &symbols);
  std::vector<WATExpr> EmitWhile(SymbolTable const &symbols);
  std::vector<WATExpr> EmitFunction(SymbolTable const &symbols);
};
