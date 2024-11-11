#include <format>
#include <stdexcept>

#include "ASTNode.hpp"
#include "Error.hpp"

std::vector<WATExpr> ASTNode::Emit(SymbolTable const &symbols) const {
  switch (type) {
  case SCOPE:
    return EmitScope(symbols);
  case FUNCTION:
    return EmitFunction(symbols);
  case ASSIGN:
    return EmitAssign(symbols);
  case IDENTIFIER:
    return EmitIdentifier(symbols);
  case CONDITIONAL:
    return EmitConditional(symbols);
  case OPERATION:
    return EmitOperation(symbols);
  case LITERAL:
    return EmitLiteral(symbols);
  case WHILE:
    return EmitWhile(symbols);
  case RETURN: {
    assert(children.size() == 1);
    return children.at(0).Emit(symbols);
  }
  case EMPTY:
  case MODULE: // module should be called manually on root node
  default:
    assert(false);
    return {};
  };
}

WATExpr ASTNode::EmitModule(SymbolTable const &symbols) const {
  assert(type == ASTNode::MODULE);
  WATExpr out{"module"};
  for (ASTNode const &child : children) {
    out.AddChildren(child.Emit(symbols));
  }
  for (FunctionInfo const &func : symbols.functions) {
    WATExpr &export_expr = out.Child("export", Quote(func.name));
    export_expr.Child("func", "$" + func.name);
  }
  return out;
}

std::vector<WATExpr> ASTNode::EmitLiteral(SymbolTable const &symbols) const {
  return {WATExpr{"i32.const", // TODO: use real type
                  {std::format("{}", value)},
                  {},
                  "Literal value"}};
}

std::vector<WATExpr> ASTNode::EmitScope(SymbolTable const &symbols) const {
  // push a new scope
  // run each child node in order
  // pop scope
  // for (ASTNode &child : children) {
  //   child.Emit(symbols);
  // }
  ErrorNoLine("Not implemented");
}

std::vector<WATExpr> ASTNode::EmitAssign(SymbolTable const &symbols) const {
  // assert(children.size() == 2);
  // double rvalue = children.at(1).EmitExpect(symbols);
  // symbols.SetValue(children.at(0).var_id, rvalue);
  // return rvalue;

  assert(children.size() == 2);
  assert(children[0].type == IDENTIFIER);

  std::string var_name = "$var" + std::to_string(children[0].var_id);
  WATExpr rvalue = (children[1].Emit(symbols))[0]; // this should produce some code which, when run, leaves the rvalue on the stack
  
  return {WATExpr{"local.set", {var_name}, {rvalue}}};

  ErrorNoLine("Not implemented");
}

std::vector<WATExpr> ASTNode::EmitIdentifier(SymbolTable const &symbols) const {
  // assert(value == double{});
  // assert(literal == std::string{});

  // return symbols.GetValue(var_id, token);
  return {WATExpr{"local.get", "$var" + std::to_string(this->var_id)}};
  ErrorNoLine("Not implemented");
}

std::vector<WATExpr>
ASTNode::EmitConditional(SymbolTable const &symbols) const {
  // conditional statement is of the form "if (expression1) statment1 else
  // statement2" so a conditional node should have 2 or 3 children: an
  // expression, a statement, and possibly another statement run the first
  // one; if it gives a nonzero value, run the second; otherwise, run the
  // third, if it exists
  // assert(children.size() == 2 || children.size() == 3);

  // double condition = children[0].EmitExpect(symbols);
  // if (condition != 0) {
  //   children[1].Emit(symbols);
  //   return;
  // }

  // if (children.size() < 3) {
  //   return;
  // }

  // children[2].Emit(symbols);
  assert(children.size() == 2 || children.size() == 3);
  std::vector<WATExpr> condition = children[0].Emit(symbols);
  WATExpr if_then_else{"if"};

  if (children.size() == 3 && children[1].type == RETURN && children[2].type == RETURN){
    if_then_else.Child(WATExpr{"result", "i32"}); // another awful hack
  }

  WATExpr then = WATExpr{"then", {}, children[1].Emit(symbols)};
  if (children[1].type == RETURN){
    then.Child(WATExpr{"br", "$fun_exit"}); // awful hack
  }
  if_then_else.Child(then);
  if (children.size() == 3){
    WATExpr else_expr{"else", {}, children[2].Emit(symbols)};
    if (children[1].type == RETURN){
      else_expr.Child(WATExpr{"br", "$fun_exit"}); // awful hack
    }
    if_then_else.Child(else_expr);
  }
  condition.push_back(if_then_else);
  return condition;

  // ErrorNoLine("Not implemented");
}

std::vector<WATExpr> ASTNode::EmitOperation(SymbolTable const &symbols) const {
  // node will have an operator (e.g. +, *, etc.) specified somewhere (maybe
  // in the "literal"?) and one or two children run the child or children,
  // apply the operator to the returned value(s), then return the result
  // assert(children.size() >= 1);
  // double left = children.at(0).EmitExpect(symbols);
  // if (literal == "!") {
  //   return left == 0 ? 1 : 0;
  // }
  // if (literal == "-" && children.size() == 1) {
  //   return -1 * left;
  // }
  // assert(children.size() == 2);
  // if (literal == "&&") {
  //   if (!left)
  //     return 0; // short-circuit when left is false
  //   return children[1].EmitExpect(symbols) != 0;
  // } else if (literal == "||") {
  //   if (left)
  //     return 1; // short-circuit when left is true
  //   return children[1].EmitExpect(symbols) != 0;
  // }
  // // don't evaluate the right until you know you won't have to short-circuit
  // double right = children.at(1).EmitExpect(symbols);
  // if (literal == "**") {
  //   return std::pow(left, right);
  // } else if (literal == "*") {
  //   return left * right;
  // } else if (literal == "/") {
  //   if (right == 0) {
  //     ErrorNoLine("Division by zero");
  //   }
  //   return left / right;
  // } else if (literal == "%") {
  //   if (right == 0) {
  //     ErrorNoLine("Modulus by zero");
  //   }
  //   return static_cast<int>(left) % static_cast<int>(right);
  // } else if (literal == "+") {
  //   return left + right;
  // } else if (literal == "-") {
  //   return left - right;
  // } else if (literal == "<") {
  //   return left < right;
  // } else if (literal == ">") {
  //   return left > right;
  // } else if (literal == "<=") {
  //   return left <= right;
  // } else if (literal == ">=") {
  //   return left >= right;
  // } else if (literal == "==") {
  //   return left == right;
  // } else if (literal == "!=") {
  //   return left != right;
  // } else {
  //   std::string message = "Tried to run unknown operator ";
  //   message.append(literal);
  //   throw std::runtime_error(message);
  // }
  assert(children.size() >= 1);
  std::vector<WATExpr> left = children.at(0).Emit(symbols);

  if (literal == "!"){
    WATExpr cond{"if"};
    WATExpr ret_type{"result i32"};
    WATExpr ret0{"then", "i32.const 0"};
    WATExpr ret1{"else", "i32.const 1"};

    cond.AddChildren({ret_type});
    cond.AddChildren({ret0});
    cond.AddChildren({ret1});

    std::vector<WATExpr> expr = left;
    expr.push_back(cond);
    return expr;
  } else if (literal == "-"){
    WATExpr expr{"i32.mul"};
    WATExpr negative_one{"i32.const", "-1"};
    expr.AddChildren({negative_one});
    expr.AddChildren(left);
    return {expr};
  }
  std::vector<WATExpr> right = children.at(1).Emit(symbols);
  if (literal == "+"){
    WATExpr expr{"i32.add"};
    expr.AddChildren(left);
    expr.AddChildren(right);
    return {expr};
  } else if (literal == "-") {
    WATExpr expr{"i32.sub"};
    expr.AddChildren(left);
    expr.AddChildren(right);
    return {expr};
  } else if (literal == "*") {
    WATExpr expr{"i32.mul"};
    expr.AddChildren(left);
    expr.AddChildren(right);
    return {expr};
  // handle division later
  } else if (literal == "%"){
    WATExpr expr{"i32.rem_u"};
    expr.AddChildren(left);
    expr.AddChildren(right);
    return {expr};
  } else if (literal == "<") {
    WATExpr expr{"i32.lt_s"};
    expr.AddChildren(left);
    expr.AddChildren(right);
    return {expr};
  } else if (literal == ">") {
    WATExpr expr{"i32.gt_s"};
    expr.AddChildren(left);
    expr.AddChildren(right);
    return {expr};
  } else if (literal == "<="){
    WATExpr expr{"i32.le_s"};
    expr.AddChildren(left);
    expr.AddChildren(right);
    return {expr};
  } else if (literal == ">="){
    WATExpr expr{"i32.ge_s"};
    expr.AddChildren(left);
    expr.AddChildren(right);
    return {expr};
  }
  ErrorNoLine("Not implemented");
}

std::vector<WATExpr> ASTNode::EmitWhile(SymbolTable const &symbols) const {
  // assert(children.size() == 2);
  // assert(value == double{});
  // assert(literal == std::string{});

  // ASTNode &condition = children[0];
  // ASTNode &body = children[1];
  // while (condition.EmitExpect(symbols)) const {
  //   body.Emit(symbols);
  // }
  ErrorNoLine("Not implemented");
}

std::vector<WATExpr> ASTNode::EmitFunction(SymbolTable const &symbols) const {
  FunctionInfo const &info = symbols.functions.at(var_id);
  WATExpr function{"func", "$" + info.name};
  for (auto const &[name, var_id] : info.arguments) {
    VariableInfo const &var_info = symbols.variables.at(var_id);
    function.Child("param", "$var" + std::to_string(var_id),
                   var_info.type.WATType());
  }
  // add result to function and its exit block
  WATExpr result{"result", info.rettype.WATType()};
  function.Child(result);

  WATExpr &block = function.Child("block", "$fun_exit");
  block.Child(result);

  for (ASTNode const &child : children) {
    block.AddChildren(child.Emit(symbols));
  }
  return {function};
}
