#include "ASTNode.hpp"


std::optional<double> ASTNode::Emit(SymbolTable &symbols) {
  switch (type) {
  case EMPTY:
    return std::nullopt;
  case SCOPE:
    EmitScope(symbols);
    return std::nullopt;
  case ASSIGN:
    return EmitAssign(symbols);
  case IDENTIFIER:
    return EmitIdentifier(symbols);
  case CONDITIONAL:
    EmitConditional(symbols);
    return std::nullopt;
  case OPERATION:
    return EmitOperation(symbols);
  case NUMBER:
    return value;
  case WHILE:
    EmitWhile(symbols);
    return std::nullopt;
  default:
    assert(false);
    return std::nullopt; // rose: thank you gcc very cool
  };
}

double ASTNode::EmitExpect(SymbolTable &symbols) {
  if (auto result = Emit(symbols)) {
    return result.value();
  }
  throw std::runtime_error("Child did not return value!");
}

// note: I would have made some of these SymbolTable references constant, but
// they'll be making recursive calls to Emit, some of which will need to make
// changes to the symbol table, so none of them are constant except the one in
// EmitIdentifier
void ASTNode::EmitScope(SymbolTable &symbols) {
  // push a new scope
  // run each child node in order
  // pop scope
  for (ASTNode &child : children) {
    child.Emit(symbols);
  }
}
double ASTNode::EmitAssign(SymbolTable &symbols) {
  assert(children.size() == 2);
  double rvalue = children.at(1).EmitExpect(symbols);
  symbols.SetValue(children.at(0).var_id, rvalue);
  return rvalue;
}

double ASTNode::EmitIdentifier(SymbolTable &symbols) {
  assert(value == double{});
  assert(literal == std::string{});

  return symbols.GetValue(var_id, token);
}

void ASTNode::EmitConditional(SymbolTable &symbols) {
  // conditional statement is of the form "if (expression1) statment1 else
  // statement2" so a conditional node should have 2 or 3 children: an
  // expression, a statement, and possibly another statement run the first
  // one; if it gives a nonzero value, run the second; otherwise, run the
  // third, if it exists
  assert(children.size() == 2 || children.size() == 3);

  double condition = children[0].EmitExpect(symbols);
  if (condition != 0) {
    children[1].Emit(symbols);
    return;
  }

  if (children.size() < 3) {
    return;
  }

  children[2].Emit(symbols);
}

double ASTNode::EmitOperation(SymbolTable &symbols) {
  // node will have an operator (e.g. +, *, etc.) specified somewhere (maybe
  // in the "literal"?) and one or two children run the child or children,
  // apply the operator to the returned value(s), then return the result
  assert(children.size() >= 1);
  double left = children.at(0).EmitExpect(symbols);
  if (literal == "!") {
    return left == 0 ? 1 : 0;
  }
  if (literal == "-" && children.size() == 1) {
    return -1 * left;
  }
  assert(children.size() == 2);
  if (literal == "&&") {
    if (!left)
      return 0; // short-circuit when left is false
    return children[1].EmitExpect(symbols) != 0;
  } else if (literal == "||") {
    if (left)
      return 1; // short-circuit when left is true
    return children[1].EmitExpect(symbols) != 0;
  }
  // don't evaluate the right until you know you won't have to short-circuit
  double right = children.at(1).EmitExpect(symbols);
  if (literal == "**") {
    return std::pow(left, right);
  } else if (literal == "*") {
    return left * right;
  } else if (literal == "/") {
    if (right == 0) {
      ErrorNoLine("Division by zero");
    }
    return left / right;
  } else if (literal == "%") {
    if (right == 0) {
      ErrorNoLine("Modulus by zero");
    }
    return static_cast<int>(left) % static_cast<int>(right);
  } else if (literal == "+") {
    return left + right;
  } else if (literal == "-") {
    return left - right;
  } else if (literal == "<") {
    return left < right;
  } else if (literal == ">") {
    return left > right;
  } else if (literal == "<=") {
    return left <= right;
  } else if (literal == ">=") {
    return left >= right;
  } else if (literal == "==") {
    return left == right;
  } else if (literal == "!=") {
    return left != right;
  } else {
    std::string message = "Tried to run unknown operator ";
    message.append(literal);
    throw std::runtime_error(message);
  }
}

void ASTNode::EmitWhile(SymbolTable &symbols) {
  assert(children.size() == 2);
  assert(value == double{});
  assert(literal == std::string{});

  ASTNode &condition = children[0];
  ASTNode &body = children[1];
  while (condition.EmitExpect(symbols)) {
    body.Emit(symbols);
  }
}
