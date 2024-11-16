#include <format>
#include <ranges>

#include "ASTNode.hpp"
#include "Error.hpp"
#include "Value.hpp"
#include "util.hpp"

VarType ASTNode::ReturnType(SymbolTable const &table) const {
  switch (type) {
  case LITERAL:
    return value.value().getType();
  case OPERATION:
    return VarType::INT; // TODO
  case IDENTIFIER:
    return table.variables.at(var_id).type_var;
  case CONDITIONAL: {
    assert(children.size() == 2 || children.size() == 3);
    if (children.size() == 2) // only true branch, so false branch always none
      return VarType::NONE;
    VarType then_type = children.at(1).ReturnType(table);
    if (then_type == children.at(2).ReturnType(table)) {
      return then_type; // same return type, use it
    } else {
      return VarType::NONE; // different return type, ignore
    }
  }
  case RETURN:
    assert(children.size() == 1);
    return children.at(0).ReturnType(table);
  case SCOPE:
    if (children.size() == 0)
      return VarType::NONE;
    return children.back().ReturnType(table);
  case CONTINUE:
  case BREAK:
    return VarType::NONE;
  default:
    return VarType::UNKNOWN;
  }
}

std::vector<WATExpr> ASTNode::Emit(State &state) const {
  switch (type) {
  case SCOPE:
    return EmitScope(state);
  case FUNCTION:
    return EmitFunction(state);
  case ASSIGN:
    return EmitAssign(state);
  case IDENTIFIER:
    return EmitIdentifier(state);
  case CONDITIONAL:
    return EmitConditional(state);
  case OPERATION:
    return EmitOperation(state);
  case LITERAL:
    return EmitLiteral(state);
  case WHILE:
    return EmitWhile(state);
  case BREAK:
    return EmitBreak(state);
  case CONTINUE:
    return EmitContinue(state);
  case RETURN: {
    assert(children.size() == 1);
    WATExpr ret{"return"};
    ret.AddChildren(children.at(0).Emit(state));
    return ret;
  }
  case EMPTY:
    return {};
  case MODULE: // module should be called manually on root node
  default:
    assert(false);
    return {};
  };
}

WATExpr ASTNode::EmitModule(State &state) const {
  assert(type == ASTNode::MODULE);
  WATExpr out{"module"};
  for (ASTNode const &child : children) {
    out.AddChildren(child.Emit(state));
  }
  for (FunctionInfo const &func : state.table.functions) {
    out.Child("export", Quote(func.name))
        .Child("func", Variable(func.name))
        .Inline();
  }
  return out;
}

std::vector<WATExpr> ASTNode::EmitLiteral(State &symbols) const {
  std::string valueStr = std::visit(
      [](auto &&value) { return std::format("{}", value); }, value->getValue());
  return WATExpr("i32.const", std::format("{}", valueStr))
      .Comment("Literal value")
      .Inline();
}

std::vector<WATExpr> ASTNode::EmitScope(State &state) const {
  // push a new scope
  // run each child node in order
  // pop scope
  // for (ASTNode &child : children) {
  //   child.Emit(state);
  // }
  std::vector<WATExpr> new_scope{};
  for (ASTNode const &child : children) {
    std::vector<WATExpr> child_exprs = child.Emit(state);
    for (WATExpr expr : child_exprs) {
      new_scope.push_back(expr);
    }
  }
  return new_scope;
}

std::vector<WATExpr> ASTNode::EmitAssign(State &state) const {
  assert(children.size() == 2);
  assert(children[0].type == IDENTIFIER);

  // this should produce some code which, when run, leaves the
  // rvalue on the stack
  WATExpr rvalue = (children[1].Emit(state))[0];

  return WATExpr{"local.set", {Variable("var", children[0].var_id)}, {rvalue}};
}

std::vector<WATExpr> ASTNode::EmitIdentifier(State &state) const {
  return WATExpr{"local.get", Variable("var", var_id)};
}

std::vector<WATExpr> ASTNode::EmitConditional(State &state) const {
  assert(children.size() == 2 || children.size() == 3);
  std::vector<WATExpr> condition = children[0].Emit(state);
  WATExpr if_then_else{"if"};

  VarType rettype = ReturnType(state.table);
  // TODO: remove the "unknown" check here
  if (rettype != VarType::NONE && rettype != VarType::UNKNOWN) {
    if_then_else.Child("result", rettype.WATType()).Inline();
  }

  WATExpr then = WATExpr{"then", {}, children[1].Emit(state)};
  if_then_else.Child(then);

  if (children.size() == 3) {
    WATExpr else_expr{"else", {}, children[2].Emit(state)};
    if_then_else.Child(else_expr);
  }
  condition.push_back(if_then_else);
  return condition;
}

std::vector<WATExpr> ASTNode::EmitOperation(State &state) const {
  assert(children.size() >= 1);
  std::vector<WATExpr> left = children.at(0).Emit(state);

  if (literal == "!") {
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
  } else if (literal == "-") {
    WATExpr expr{"i32.mul"};
    WATExpr negative_one{"i32.const", "-1"};
    expr.AddChildren({negative_one});
    expr.AddChildren(left);
    return expr;
  } else if (literal == "sqrt") {
    ErrorNoLine("Not implemeneted (sqrt)");
  }

  // remaining operations are binary operations
  assert(children.size() == 2);
  std::vector<WATExpr> right = children.at(1).Emit(state);

  if (literal == "+") {
    WATExpr expr{"i32.add"};
    expr.AddChildren(left);
    expr.AddChildren(right);
    return expr;
  } else if (literal == "-") {
    WATExpr expr{"i32.sub"};
    expr.AddChildren(left);
    expr.AddChildren(right);
    return expr;
  } else if (literal == "*") {
    WATExpr expr{"i32.mul"};
    expr.AddChildren(left);
    expr.AddChildren(right);
    return expr;
    // handle division later
  } else if (literal == "%") {
    WATExpr expr{"i32.rem_u"};
    expr.AddChildren(left);
    expr.AddChildren(right);
    return expr;
  } else if (literal == "<") {
    WATExpr expr{"i32.lt_s"};
    expr.AddChildren(left);
    expr.AddChildren(right);
    return expr;
  } else if (literal == ">") {
    WATExpr expr{"i32.gt_s"};
    expr.AddChildren(left);
    expr.AddChildren(right);
    return expr;
  } else if (literal == "<=") {
    WATExpr expr{"i32.le_s"};
    expr.AddChildren(left);
    expr.AddChildren(right);
    return expr;
  } else if (literal == ">=") {
    WATExpr expr{"i32.ge_s"};
    expr.AddChildren(left);
    expr.AddChildren(right);
    return expr;
  } else if (literal == "==") {
    WATExpr expr{"i32.eq"};
    expr.AddChildren(left);
    expr.AddChildren(right);
    return expr;
  } else if (literal == "!=") {
    WATExpr expr{"i32.ne"};
    expr.AddChildren(left);
    expr.AddChildren(right);
    return expr;
  }
  ErrorNoLine("Not implemented (emit operation fallthrough)");
}

std::vector<WATExpr> ASTNode::EmitWhile(State &state) const {
  assert(children.size() == 2);
  // make labels ahead of time for ease of use
  std::string const loop_label = join(state.loop_idx, ".");
  std::string const loop_id = Variable("loop_", loop_label);
  std::string const loop_exit = Variable("loop_exit_", loop_label);

  state.loop_idx.back()++;
  state.loop_idx.push_back(0);

  // create loop block
  WATExpr block{"block", loop_exit};
  WATExpr &loop = block.Child("loop", loop_id);

  // construct conditional
  std::vector<WATExpr> condition = children.at(0).Emit(state);
  loop.Child("br_if", loop_exit)
      .Comment("Check while loop condition", false)
      .Child("i32.eqz")
      .Comment("Invert condition, break if condition false", false)
      .AddChildren(condition);

  loop.AddChildren(children.at(1).Emit(state));
  loop.Child("br", loop_id).Comment("Jump to start of while loop");

  state.loop_idx.pop_back();

  return block;
}

std::vector<WATExpr> ASTNode::EmitContinue(State &state) const {
  std::string const loop_label = join(state.loop_idx, ".");
  return WATExpr{"br", Variable("loop_", loop_label)};
}

std::vector<WATExpr> ASTNode::EmitBreak(State &state) const {
  std::string const loop_label = join(state.loop_idx, ".");
  return WATExpr{"br", Variable("loop_exit_", loop_label)};
}

std::vector<WATExpr> ASTNode::EmitFunction(State &state) const {
  FunctionInfo const &info = state.table.functions.at(var_id);
  WATExpr function{"func", Variable(info.name)};
  function.format.newline = true;

  // write out parameters (first info.parameters values in info.variables)
  for (size_t var_id : info.variables | std::views::take(info.parameters)) {
    VariableInfo const &param = state.table.variables.at(var_id);
    function.Child("param", Variable("var", var_id), param.type_var.WATType())
        .Inline();
  }

  // add result to function
  WATExpr result = WATExpr("result", info.rettype.WATType()).Inline();
  function.Child(result);

  // write out locals (remaining values in info.variables)
  for (size_t var_id : info.variables | std::views::drop(info.parameters)) {
    VariableInfo const &var = state.table.variables.at(var_id);
    function.Child("local", Variable("var", var_id), var.type_var.WATType())
        .Comment("Declare " + var.type_var.TypeName() + " " + var.name);
  }

  for (ASTNode const &child : children) {
    function.AddChildren(child.Emit(state));
  }

  return function;
}
