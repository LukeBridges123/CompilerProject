#include <cassert>
#include <fstream>
#include <memory>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "ASTNode.hpp"
#include "Error.hpp"
#include "State.hpp"
#include "Type.hpp"
#include "WAT.hpp"
#include "lexer.hpp"

using namespace emplex;

class Tubular {
private:
  std::vector<Token> tokens{};
  emplex::Lexer lexer{};
  State state{};
  size_t token_idx{0};
  ASTNode root{ASTNode::MODULE};
  size_t loop_depth = 0;

  Token const &CurToken() const {
    if (token_idx >= tokens.size())
      ErrorNoLine("Unexpected EOF");
    return tokens.at(token_idx);
  }

  Token const &ConsumeToken() {
    if (token_idx >= tokens.size())
      ErrorNoLine("Unexpected EOF");
    return tokens.at(token_idx++);
  }

  Token const &ExpectToken(int token) {
    if (CurToken() == token) {
      return ConsumeToken();
    }
    ErrorUnexpected(CurToken(), token);
  }

  // rose: C++ optionals can't hold references, grumble grumble
  Token const *IfToken(int token) {
    if (token_idx < tokens.size() && CurToken() == token) {
      return &ConsumeToken();
    }
    return nullptr;
  }

  void ParseBlock(ASTNode &block) {
    ExpectToken(Lexer::ID_SCOPE_START);
    while (CurToken() != Lexer::ID_SCOPE_END) {
      block.AddChild(ParseStatement());
    }
    ConsumeToken();
  }

  ASTNode ParseFunction() {
    ExpectToken(Lexer::ID_FUNCTION);

    Token func_name = ExpectToken(Lexer::ID_ID);
    size_t func_id =
        state.table.AddFunction(func_name.lexeme, func_name.line_id);
    FunctionInfo &func_info = state.table.functions.at(func_id);
    ASTNode function{ASTNode::FUNCTION, func_id};

    state.table.PushScope();

    ExpectToken(Lexer::ID_OPEN_PARENTHESIS);

    // parse arguments
    while (CurToken() != Lexer::ID_CLOSE_PARENTHESIS) {
      VarType var_type = ExpectToken(Lexer::ID_TYPE);
      Token var_name = ExpectToken(Lexer::ID_ID);

      state.table.AddVar(var_name.lexeme, var_type, var_name.line_id);
      func_info.parameters++;

      IfToken(','); // consume comma if exists
    }
    ConsumeToken(); // close parenthesis

    // parse return type
    ExpectToken(':');
    func_info.rettype = ExpectToken(Lexer::ID_TYPE);

    // parse body
    ParseBlock(function);

    return function;
  }

  ASTNode ParseScope() {
    ASTNode scope{ASTNode::SCOPE};
    state.table.PushScope();
    ParseBlock(scope);
    state.table.PopScope();
    return scope;
  }

  ASTNode ParseDecl() {
    VarType const &var_type = ExpectToken(Lexer::ID_TYPE);
    Token const &ident = ExpectToken(Lexer::ID_ID);
    if (IfToken(Lexer::ID_ENDLINE)) {
      state.table.AddVar(ident.lexeme, var_type, ident.line_id);
      return ASTNode{};
    }
    ExpectToken(Lexer::ID_ASSIGN);

    ASTNode expr = ParseExpr();
    ExpectToken(Lexer::ID_ENDLINE);
    VarType right_type = expr.ReturnType(state.table);
    if (var_type < right_type) {
      Error(
          CurToken(),
          "Tried to assign higher-precision value to lower-precision variable");
    }

    // don't add until _after_ we possibly resolve idents in expression
    // ex. var foo = foo should error if foo is undefined
    size_t var_id = state.table.AddVar(ident.lexeme, var_type, ident.line_id);

    ASTNode out = ASTNode{ASTNode::ASSIGN};
    out.AddChildren(ASTNode(ASTNode::IDENTIFIER, var_id), std::move(expr));

    return out;
  }

  ASTNode ParseExpr() { return ParseAssign(); }

  ASTNode ParseAssign() {
    ASTNode lhs = ParseOr();
    if (CurToken().lexeme == "=") {
      // can only have variable names as the LHS of an assignment
      if (lhs.type != ASTNode::IDENTIFIER &&
          lhs.type != ASTNode::STRING_INDEX) {
        ErrorUnexpected(CurToken(), Lexer::ID_ID, Lexer::ID_BRACKET_OPEN);
      }
      ExpectToken(Lexer::ID_ASSIGN);
      ASTNode rhs = ParseAssign();
      VarType left_type = lhs.ReturnType(state.table);
      VarType right_type = rhs.ReturnType(state.table);
      if (left_type == VarType::STRING && right_type != VarType::STRING &&
          right_type != VarType::CHAR) {
        Error(CurToken(), "Only string and char can be assigned to string");
      } else if (left_type < right_type) {
        Error(CurToken(), "Tried to assign higher-precision value to "
                          "lower-precision variable");
      }
      return ASTNode(ASTNode::ASSIGN, "=", std::move(lhs), std::move(rhs));
    }
    return lhs;
  }

  ASTNode ParseOr() {
    auto lhs = std::make_unique<ASTNode>(ParseAnd());
    while (CurToken().lexeme == "||") {
      ConsumeToken();
      ASTNode rhs = ParseAnd();

      if (lhs->ReturnType(state.table) != VarType::INT ||
          rhs.ReturnType(state.table) != VarType::INT) {
        Error(CurToken(), "Used non-int value in an or expression");
      }
      lhs = std::make_unique<ASTNode>(
          ASTNode(ASTNode::OPERATION, "||", std::move(*lhs), std::move(rhs)));
    }
    return ASTNode{std::move(*lhs)};
  }

  ASTNode ParseAnd() {
    auto lhs = std::make_unique<ASTNode>(ParseEquals());
    while (CurToken().lexeme == "&&") {
      ConsumeToken();
      ASTNode rhs = ParseEquals();
      if (lhs->ReturnType(state.table) != VarType::INT ||
          rhs.ReturnType(state.table) != VarType::INT) {
        Error(CurToken(), "Used non-int value in an and expression");
      }
      lhs = std::make_unique<ASTNode>(
          ASTNode(ASTNode::OPERATION, "&&", std::move(*lhs), std::move(rhs)));
    }
    return ASTNode{std::move(*lhs)};
  }

  ASTNode ParseEquals() {
    auto lhs = std::make_unique<ASTNode>(ParseCompare());
    if (CurToken() == Lexer::ID_EQUALS) {
      std::string operation = ExpectToken(Lexer::ID_EQUALS).lexeme;
      ASTNode rhs = ParseCompare();
      return ASTNode(ASTNode::OPERATION, operation, std::move(*lhs),
                     std::move(rhs));
    }
    return ASTNode{std::move(*lhs)};
  }

  ASTNode ParseCompare() {
    auto lhs = std::make_unique<ASTNode>(ParseAddSub());
    if (CurToken() == Lexer::ID_COMPARE) {
      std::string operation = ExpectToken(Lexer::ID_COMPARE).lexeme;
      ASTNode rhs = ParseAddSub();
      return ASTNode(ASTNode::OPERATION, operation, std::move(*lhs),
                     std::move(rhs));
    }
    return ASTNode{std::move(*lhs)};
  }

  ASTNode ParseAddSub() {
    auto lhs = std::make_unique<ASTNode>(ParseMulDivMod());
    while (CurToken().lexeme == "+" || CurToken().lexeme == "-") {
      std::string operation = ConsumeToken().lexeme;
      ASTNode rhs = ParseMulDivMod();
      lhs = std::make_unique<ASTNode>(ASTNode(ASTNode::OPERATION, operation,
                                              std::move(*lhs), std::move(rhs)));
    }
    return ASTNode{std::move(*lhs)};
  }

  ASTNode ParseMulDivMod() {
    auto lhs = std::make_unique<ASTNode>(ParseTerm());

    while (CurToken().lexeme == "*" || CurToken().lexeme == "/" ||
           CurToken().lexeme == "%") {
      std::string operation = ConsumeToken().lexeme;
      ASTNode rhs = ParseTerm();

      VarType lhs_type = lhs->ReturnType(state.table);
      VarType rhs_type = rhs.ReturnType(state.table);
      if ((lhs_type == VarType::CHAR || lhs_type == VarType::STRING) &&
          (rhs_type == VarType::CHAR || rhs_type == VarType::STRING)) {
        Error(CurToken(), "Invalid action: Cannot perform multiplication, "
                          "division, or modulus on a char or a string with "
                          "another char or string!");
      } else if ((lhs_type == VarType::CHAR || lhs_type == VarType::CHAR ||
                  rhs_type == VarType::STRING || rhs_type == VarType::STRING) &&
                 operation != "*") {
        Error(CurToken(), "Invalid action: Cannot perform "
                          "division, or modulus on a char or string type!");
      } else if ((lhs_type == VarType::CHAR || lhs_type == VarType::CHAR ||
                  rhs_type == VarType::STRING || rhs_type == VarType::STRING) &&
                 (lhs_type == VarType::DOUBLE || rhs_type == VarType::DOUBLE)) {
        Error(CurToken(), "Invalid action: Cannot perform "
                          "operation on a char or string type with a double!");
      }

      if (operation == "%" &&
          (lhs_type == VarType::DOUBLE || rhs_type == VarType::DOUBLE)) {
        Error(CurToken(),
              "Invalid action: Cannot perform modulus with a double type!");
      }

      lhs = std::make_unique<ASTNode>(ASTNode(ASTNode::OPERATION, operation,
                                              std::move(*lhs), std::move(rhs)));
    }
    return ASTNode{std::move(*lhs)};
  }

  bool isStringOrChar(ASTNode node) {
    // std::cout << "testing..." << std::endl;
    if (node.ReturnType(state.table) == VarType::CHAR ||
        node.ReturnType(state.table) == VarType::STRING) {
      return true;
    }
    return false;
  }

  ASTNode ParseNegate() {
    auto lhs = std::make_unique<ASTNode>(ASTNode::LITERAL, Value{-1});
    Token const &curr_token = CurToken();
    auto rhs = ParseTerm();

    if (rhs.ReturnType(state.table) == VarType::CHAR) {
      Error(curr_token, "Invalid action: Cannot negate a char type!");
    }

    return ASTNode(ASTNode::OPERATION, "*", std::move(*lhs), std::move(rhs));
  }

  ASTNode ParseNOT() {
    Token const curr_token = CurToken();
    auto rhs = ParseTerm();

    if (rhs.ReturnType(state.table) != VarType::INT) {
      Error(curr_token, "Invalid action: Cannot perform a logical \"NOT\" on a "
                        "type thats not an INT!");
    }

    return ASTNode(ASTNode::OPERATION, "!", std::move(rhs));
  }

  ASTNode ParseSqrt() {
    return ASTNode(ASTNode::BUILT_IN_FUNCTION_CALL, "sqrt", ParseExpr());
  }

  ASTNode CheckTypeCast(ASTNode node) {
    if (CurToken() != Lexer::ID_TYPE_CAST) {
      return node;
    }
    Token const &token = ConsumeToken();

    if (token.lexeme == ":int") {
      ASTNode out{ASTNode::CAST_INT};
      out.AddChildren(std::move(node));
      return out;
    }

    if (token.lexeme == ":double") {
      ASTNode out{ASTNode::CAST_DOUBLE};
      out.AddChildren(std::move(node));
      return out;
    }

    if (token.lexeme == ":char") {
      ASTNode out{ASTNode::CAST_CHAR};
      out.AddChildren(std::move(node));
      return out;
    }

    if (token.lexeme == ":string") {
      ASTNode out{ASTNode::CAST_STRING};
      out.AddChildren(std::move(node));
      return out;
    }

    Error(token, "Attempt to cast to unknown type ", token.lexeme.substr(1));
  }

  ASTNode ParseIdentifier() {

    std::string name = ConsumeToken().lexeme;

    if (IfToken(Lexer::ID_OPEN_PARENTHESIS)) {
      if (name == "size") {
        ASTNode out{ASTNode::BUILT_IN_FUNCTION_CALL, name};
        ASTNode arg = ParseExpr();
        out.AddChild(std::move(arg));
        if (arg.ReturnType(state.table) != VarType::STRING) {
          ErrorNoLine(
              "Invalid: Attempting to use size() on a non-string type.");
        }
        ExpectToken(Lexer::ID_CLOSE_PARENTHESIS);
        return out;
      }
      size_t id = state.table.FindFunction(name, CurToken().line_id);
      ASTNode out{ASTNode::FUNCTION_CALL, id};

      std::vector<VarType> arg_types{};
      while (CurToken() != Lexer::ID_CLOSE_PARENTHESIS) {
        ASTNode arg = ParseExpr();
        arg_types.push_back(arg.ReturnType(state.table));
        out.AddChild(std::move(arg));
        IfToken(',');
      }
      ConsumeToken();
      if (!state.table.CheckTypes(id, arg_types, CurToken().line_id)) {
        Error(CurToken().line_id, "Incorrect types in function call");
      }
      return out;
    } else {
      return ASTNode(ASTNode::IDENTIFIER,
                     state.table.FindVar(name, CurToken().line_id));
    }
  }

  ASTNode ParseString() {
    Token const &token = ExpectToken(Lexer::ID_STRING);
    size_t string_pos =
        state.AddString(token.lexeme.substr(1, token.lexeme.size() - 2));
    return ASTNode{ASTNode::LITERAL, Value{string_pos}};
  }

  template <typename T> ASTNode ConstructLiteral(T value) {
    return CheckTypeCast(ASTNode(ASTNode::LITERAL, Value{value}));
  }

  ASTNode String_ops(ASTNode node) {
    if (CurToken() == Lexer::ID_TYPE_CAST) {
      return CheckTypeCast(std::move(node));
    } else if (CurToken() == Lexer::ID_BRACKET_OPEN) {
      ASTNode out{ASTNode::STRING_INDEX};
      ExpectToken(Lexer::ID_BRACKET_OPEN);
      ASTNode subexpression = ParseExpr();
      ExpectToken(Lexer::ID_BRACKET_CLOSE);
      out.AddChild(std::move(node));
      out.AddChild(std::move(subexpression));
      return out;
    }
    return node;
  }

  ASTNode ParseTerm() {
    Token const &current = CurToken();
    switch (current) {
    case Lexer::ID_FLOAT:
      return ConstructLiteral(std::stod(ConsumeToken().lexeme));
    case Lexer::ID_INT:
      return ConstructLiteral(std::stoi(ConsumeToken().lexeme));
    case Lexer::ID_CHAR:
      return ConstructLiteral(ConsumeToken().lexeme[1]);
    case Lexer::ID_ID:
      return String_ops(ParseIdentifier());
    case Lexer::ID_STRING:
      return CheckTypeCast(ParseString());
    case Lexer::ID_OPEN_PARENTHESIS: {
      ExpectToken(Lexer::ID_OPEN_PARENTHESIS);
      ASTNode subexpression = ParseExpr();
      ExpectToken(Lexer::ID_CLOSE_PARENTHESIS);
      return String_ops(std::move(subexpression));
    }
    case Lexer::ID_MATH:
      if (current.lexeme == "-") {
        ConsumeToken();
        return ParseNegate();
      }
      ErrorUnexpected(current);
    case Lexer::ID_NOT:
      ConsumeToken();
      return ParseNOT();
    case Lexer::ID_SQRT: {
      ConsumeToken();
      ExpectToken(Lexer::ID_OPEN_PARENTHESIS);
      ASTNode subexpr = ParseSqrt();
      ExpectToken(Lexer::ID_CLOSE_PARENTHESIS);
      return CheckTypeCast(std::move(subexpr));
    }
    default:
      ErrorUnexpected(current);
    }
    return ASTNode{};
  }

  ASTNode ParseIf() {
    ExpectToken(Lexer::ID_IF);
    ExpectToken(Lexer::ID_OPEN_PARENTHESIS);

    if (CurToken() == Lexer::ID_CLOSE_PARENTHESIS) {
      Error(CurToken(), "Expected condition body, found empty condition");
    }

    ASTNode node{ASTNode::CONDITIONAL};

    node.AddChild(ParseExpr());

    ExpectToken(Lexer::ID_CLOSE_PARENTHESIS);

    node.AddChild(ParseStatement());

    if (IfToken(Lexer::ID_ELSE)) {
      node.AddChild(ParseStatement());
    }

    return node;
  }

  ASTNode ParseWhile() {
    ExpectToken(Lexer::ID_WHILE);
    ExpectToken(Lexer::ID_OPEN_PARENTHESIS);
    ASTNode node = ASTNode(ASTNode::WHILE);
    node.AddChild(ParseExpr());

    ExpectToken(Lexer::ID_CLOSE_PARENTHESIS);

    if (IfToken(Lexer::ID_ENDLINE)) {
      return node;
    }

    loop_depth++;
    node.AddChild(ParseStatement());
    loop_depth--;

    return node;
  }

  ASTNode ParseLoopControl() {
    if (loop_depth == 0) {
      Error(CurToken(), "Found ", CurToken().lexeme, " outside loop");
    }
    ASTNode::Type nodetype =
        CurToken() == Lexer::ID_CONTINUE ? ASTNode::CONTINUE : ASTNode::BREAK;
    ConsumeToken();
    ExpectToken(Lexer::ID_ENDLINE);
    return ASTNode{nodetype};
  }

  ASTNode ParseStatement() {
    Token const &current = CurToken();
    switch (current) {
    case Lexer::ID_FUNCTION:
      return ParseFunction();
    case Lexer::ID_SCOPE_START:
      return ParseScope();
    case Lexer::ID_TYPE:
      return ParseDecl();
    case Lexer::ID_ID:
    case Lexer::ID_FLOAT:
    case Lexer::ID_INT: {
      ASTNode node = ParseExpr();
      ExpectToken(Lexer::ID_ENDLINE);
      return node;
    }
    case Lexer::ID_RETURN: {
      ASTNode node = ASTNode{ASTNode::RETURN};
      ConsumeToken();
      node.AddChild(ParseExpr());
      ExpectToken(Lexer::ID_ENDLINE);
      return node;
    }
    case Lexer::ID_IF:
      return ParseIf();
    case Lexer::ID_WHILE:
      return ParseWhile();
    case Lexer::ID_BREAK:
    case Lexer::ID_CONTINUE:
      return ParseLoopControl();
    default:
      ErrorUnexpected(current);
    }
  }

public:
  Tubular(std::ifstream &input) {
    tokens = lexer.Tokenize(input);
    Parse();
  };

  void Parse() {
    while (token_idx < tokens.size()) {
      root.AddChild(ParseFunction());
    }
  }

  WATExpr GenerateCode() { return root.EmitModule(state); }
};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    ErrorNoLine("Format: ", argv[0], " [filename]");
  }

  std::string filename{argv[1]};
  std::ifstream in_file{filename};
  if (in_file.fail()) {
    ErrorNoLine("Unable to open file '", filename, "'.");
  }

  Tubular tube{in_file};
  tube.Parse();
  WATExpr wat = tube.GenerateCode();

  WATWriter writer{std::cout};
  writer.Write(wat);
}
