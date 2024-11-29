#include <algorithm>
#include <format>
#include <ranges>

#include "ASTNode.hpp"
#include "Error.hpp"
#include "Value.hpp"
#include "WAT.hpp"
#include "internal_wat.hpp"
#include "util.hpp"

VarType ASTNode::ReturnType(SymbolTable const &table) const {
  switch (type) {
  case LITERAL:
    return value.value().getType();
  case ASSIGN:
    assert(children.size() == 2);
    return VarType::NONE;
  case OPERATION:
    // logical/comparison operators + modulus always return an int
    if (literal == "!" || literal == "||" || literal == "&&" ||
        literal == "<" || literal == ">" || literal == "<=" ||
        literal == ">=" || literal == "==" || literal == "!=" ||
        literal == "%") {
      return VarType::INT;
    }
    // sqrt always returns a double; easiest to have / do the same thing?
    if (literal == "sqrt") {
      return VarType::DOUBLE;
    }
    if (literal == "-" && children.size() == 1) {
      return children.at(0).ReturnType(table);
    }
    // find type based on precision
    if (literal == "+" || literal == "-" || literal == "*" || literal == "/") {
      assert(children.size() == 2);
      VarType left_type = children.at(0).ReturnType(table);
      VarType right_type = children.at(1).ReturnType(table);
      return std::max(left_type, right_type);
    }
    ErrorNoLine("Invalid operation during type checking");
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
  case CAST_DOUBLE:
    return VarType::DOUBLE;
  case CAST_INT:
    return VarType::INT;
  case CAST_CHAR:
    return VarType::CHAR;
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
  case FUNCTION_CALL:
    return table.functions.at(var_id).rettype;
  default:
    assert(false);
    return VarType::UNKNOWN;
  }
}

bool ASTNode::HasReturn(State const &state) const {
  switch (type) {
  case RETURN:
    return true;
  case CONDITIONAL:
    // both true and false branches must have a return
    return children.size() == 3 && children.at(1).HasReturn(state) &&
           children.at(2).HasReturn(state);
  case SCOPE:
  case FUNCTION:
    // if any child has a return, then we have a return
    return std::ranges::any_of(children, [state](ASTNode const &child) {
      return child.HasReturn(state);
    });
  default:
    return false;
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
  case FUNCTION_CALL:
    return EmitFunctionCall(state);
  case RETURN:
    assert(children.size() == 1);
    return WATExpr{"return", children.at(0).Emit(state)};
  case CAST_INT: {
    assert(children.size() == 1);
    std::vector<WATExpr> ret = children.at(0).Emit(state);
    if (children.at(0).ReturnType(state.table) == VarType::DOUBLE) {
      ret.emplace_back("i32.trunc_f64_s");
    }
    return ret;
  }
  case CAST_DOUBLE: {
    assert(children.size() == 1);
    std::vector<WATExpr> ret = children.at(0).Emit(state);
    if (children.at(0).ReturnType(state.table) == VarType::INT) {
      ret.emplace_back("f64.convert_i32_s");
    }
    return ret;
  }
  case Cast_STRING:
    return EmitCastString(state);
  case CAST_CHAR:
    ErrorNoLine("Cast not implemented");
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

  WATParser parser{internal_wat, internal_wat_len};
  std::vector<WATExpr> internal_funcs = parser.Parse();
  bool injected = false;

  // write memory declaration and export
  out.Child("memory", WATExpr("export", Quote("memory")).Inline(), "1");

  // write string literals
  size_t current_free = 0;
  for (std::string const &literal : state.string_literals) {
    out.Child("data")
        .Push(WATExpr("i32.const", std::to_string(current_free)).Inline())
        .Push(Quote(literal + "\\00"));
    current_free += literal.size() + 1;
  }

  // write free memory position variable
  WATExpr &global = out.Child("global", Variable("_free")).Newline();
  global.Child("mut", "i32").Inline();
  global.Child("i32.const", std::to_string(state.string_pos)).Inline();

  // generate function body
  for (ASTNode const &child : children) {
    // inject our functions before writing user-defined functions
    if (!injected && child.type == ASTNode::FUNCTION) {
      out.Push(std::move(internal_funcs));
      injected = true;
    }

    out.Push(child.Emit(state));
  }

  // generate exports for functions and memory
  for (FunctionInfo const &func : state.table.functions) {
    out.Child("export", Quote(func.name))
        .Child("func", Variable(func.name))
        .Inline();
  }

  return out;
}

std::vector<WATExpr>
ASTNode::EmitLiteral([[maybe_unused]] State &symbols) const {
  std::string value_str = std::visit(
      [](auto &&value) { return std::format("{}", value); }, value->getValue());
  return WATExpr(value->getType().WATOperation("const"), value_str)
      .Comment("Literal value")
      .Inline();
}

std::vector<WATExpr> ASTNode::EmitCastString(State &state) const {
  assert(children.size() == 1);

  auto val = std::visit(
      [](auto &&value) { return value; }, children.at(0).value->getValue());
  size_t string_pos = state.AddString(std::to_string(val));
  
  ASTNode node = ASTNode{ASTNode::LITERAL, Value{string_pos}};
  
  std::vector<WATExpr> ret = node.Emit(state);

  return ret;
}

std::vector<WATExpr> ASTNode::EmitScope(State &state) const {
  std::vector<WATExpr> new_scope{};
  for (ASTNode const &child : children) {
    std::vector<WATExpr> child_exprs = child.Emit(state);
    std::ranges::move(child_exprs, std::back_inserter(new_scope));
  }
  return new_scope;
}

std::vector<WATExpr> ASTNode::EmitAssign(State &state) const {
  assert(children.size() == 2);
  assert(children[0].type == IDENTIFIER);

  // this should produce some code which, when run, leaves the
  // rvalue on the stack
  std::vector<WATExpr> rvalue;

  if (children.at(1).type == ASSIGN) {
    rvalue = children.at(1).EmitChainAssign(state);
  } else {
    rvalue = children.at(1).Emit(state);
  }

  VarType left_type = children.at(0).ReturnType(state.table);
  VarType right_type = children.at(1).ReturnType(state.table);
  if (left_type == VarType::DOUBLE && right_type == VarType::INT) {
    rvalue.emplace_back("f64.convert_i32_s");
  }
  return WATExpr{"local.set", Variable("var", children[0].var_id),
                 std::move(rvalue)};
}

std::vector<WATExpr> ASTNode::EmitChainAssign(State &state) const {
  assert(children.size() == 2);
  assert(children[0].type == IDENTIFIER);

  // this should produce some code which, when run, leaves the
  // rvalue on the stack
  std::vector<WATExpr> rvalue;

  if (children.at(1).type == ASSIGN) {
    rvalue = children.at(1).EmitChainAssign(state);
  } else {
    rvalue = children.at(1).Emit(state);
  }

  VarType left_type = children.at(0).ReturnType(state.table);
  VarType right_type = children.at(1).ReturnType(state.table);
  if (left_type == VarType::DOUBLE && right_type == VarType::INT) {
    rvalue.emplace_back("f64.convert_i32_s");
  }
  return WATExpr{"local.tee", Variable("var", children[0].var_id),
                 std::move(rvalue)};
}

std::vector<WATExpr>
ASTNode::EmitIdentifier([[maybe_unused]] State &state) const {
  return WATExpr{"local.get", Variable("var", var_id)};
}

std::vector<WATExpr> ASTNode::EmitConditional(State &state) const {
  assert(children.size() == 2 || children.size() == 3);
  std::vector<WATExpr> condition = children[0].Emit(state);
  WATExpr if_then_else{"if"};

  VarType rettype = ReturnType(state.table);
  if (rettype != VarType::NONE) {
    if_then_else.Child("result", rettype.WATType()).Inline();
  }

  if_then_else.Child("then", children[1].Emit(state));

  if (children.size() == 3) {
    if_then_else.Child("else", children[2].Emit(state));
  }
  condition.push_back(if_then_else);
  return condition;
}

std::vector<WATExpr> ASTNode::EmitOperation(State &state) const {
  assert(children.size() >= 1);
  std::vector<WATExpr> left = children.at(0).Emit(state);
  VarType left_type = children.at(0).ReturnType(state.table);

  if (literal == "!") {
    WATExpr cond = WATExpr("if")
                       .PushChild("result", "i32")
                       .PushChild("then", WATExpr{"i32.const", "0"})
                       .PushChild("else", WATExpr{"i32.const", "1"});
    left.push_back(cond);
    return left;
  } else if (literal == "-" && children.size() == 1) {
    return WATExpr{left_type.WATOperation("mul"),
                   WATExpr{left_type.WATOperation("const"), "-1"},
                   std::move(left)};
  } else if (literal == "sqrt") {
    WATExpr sqrt{"f64.sqrt", std::move(left)};
    if (left_type == VarType::INT) {
      sqrt.Child("f64.convert_i32_s");
    }

    return sqrt;
  }

  // remaining operations are binary operations
  assert(children.size() == 2);
  std::vector<WATExpr> right = children.at(1).Emit(state);
  VarType right_type = children.at(1).ReturnType(state.table);
  VarType op_type = std::max(left_type, right_type);

  if (literal == "&&") {
    WATExpr test_first{"i32.eq", WATExpr{"i32.const", "0"}, std::move(left)};
    WATExpr test_second{"i32.ne", WATExpr{"i32.const", "0"}, std::move(right)};
    WATExpr cond = WATExpr("if")
                       .PushChild("result", "i32")
                       .PushChild("then", WATExpr{"i32.const", "0"})
                       .PushChild("else", std::move(test_second));
    return {test_first, cond};
  }

  if (literal == "||") {
    WATExpr test_first{"i32.eq", WATExpr{"i32.const", "1"}, std::move(left)};
    WATExpr test_second{"i32.ne", WATExpr{"i32.const", "0"}, std::move(right)};
    WATExpr cond = WATExpr("if")
                       .PushChild("result", "i32")
                       .PushChild("then", WATExpr{"i32.const", "1"})
                       .PushChild("else", std::move(test_second));
    return {test_first, cond};
  }

  std::string op_name = LITERAL_TO_WAT.at(literal);
  bool use_signed = (literal == "<" || literal == ">" || literal == "<=" ||
                     literal == ">=" || literal == "/");
  WATExpr expr{op_type.WATOperation(op_name, use_signed)};

  expr.Push(std::move(left));
  if (left_type == VarType::INT && right_type == VarType::DOUBLE) {
    expr.Child(right_type.WATOperation("convert_i32_s"));
  }
  expr.Push(std::move(right));
  if (right_type == VarType::INT && left_type == VarType::DOUBLE) {
    expr.Child(left_type.WATOperation("convert_i32_s"));
  }

  return expr;
}

std::vector<WATExpr> ASTNode::EmitWhile(State &state) const {
  assert(children.size() == 2);

  state.loop_idx.push_back(0);
  state.loop_idx.back()++;

  // make labels ahead of time for ease of use
  std::string const loop_label = join(state.loop_idx, ".");
  std::string const loop_id = Variable("loop_", loop_label);
  std::string const loop_exit = Variable("loop_exit_", loop_label);

  // create loop block
  WATExpr block{"block", loop_exit};
  WATExpr &loop = block.Child("loop", loop_id);

  // construct conditional
  loop.Child("br_if", loop_exit)
      .Comment("Check while loop condition", false)
      .Child("i32.eqz")
      .Comment("Invert condition, break if condition false", false)
      .Push(children.at(0).Emit(state));

  loop.Push(children.at(1).Emit(state))
      .Child("br", loop_id)
      .Comment("Jump to start of while loop");

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

  if (!HasReturn(state)) {
    ErrorNoLine("Function ", info.name,
                " does not have a return statement in all control flow paths");
  }

  WATExpr function = WATExpr("func", Variable(info.name)).Newline();

  // write out parameters (first info.parameters values in info.variables)
  for (size_t var_id : info.variables | std::views::take(info.parameters)) {
    VariableInfo const &param = state.table.variables.at(var_id);
    function.Child("param", Variable("var", var_id), param.type_var.WATType())
        .Inline();
  }

  // add result to function
  function.Child("result", info.rettype.WATType()).Inline();

  // write out locals (remaining values in info.variables)
  for (size_t var_id : info.variables | std::views::drop(info.parameters)) {
    VariableInfo const &var = state.table.variables.at(var_id);
    function.Child("local", Variable("var", var_id), var.type_var.WATType())
        .Comment("Declare " + var.type_var.TypeName() + " " + var.name);
  }

  for (ASTNode const &child : children) {
    function.Push(child.Emit(state));
  }

  return function;
}

std::vector<WATExpr> ASTNode::EmitFunctionCall(State &state) const {
  std::vector<WATExpr> out{};
  for (ASTNode const &child : children) {
    std::vector<WATExpr> child_exprs = child.Emit(state);
    for (auto expr : child_exprs) {
      out.push_back(expr);
    }
  }
  out.emplace_back("call", Variable(state.table.functions.at(var_id).name));
  return out;
}
