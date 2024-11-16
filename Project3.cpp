#include <cassert>
#include <fstream>
#include <memory>
#include <string>

#include <utility>
#include <variant>
#include <vector>

#include "ASTNode.hpp"
#include "Error.hpp"
#include "State.hpp"
#include "Type.hpp"
#include "WAT.hpp"
#include "lexer.hpp"

using std::in_place_type;

using namespace emplex;

class Tubular {
private:
  std::vector<Token> tokens{};
  emplex::Lexer lexer{};
  SymbolTable table{};
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
    size_t func_id = table.AddFunction(func_name.lexeme, func_name.line_id);
    FunctionInfo &func_info = table.functions.at(func_id);
    ASTNode function{ASTNode::FUNCTION, func_id, &func_name};

    table.PushScope();

    ExpectToken(Lexer::ID_OPEN_PARENTHESIS);

    // parse arguments
    while (CurToken() != Lexer::ID_CLOSE_PARENTHESIS) {
      VarType var_type = ExpectToken(Lexer::ID_TYPE);
      Token var_name = ExpectToken(Lexer::ID_ID);

      table.AddVar(var_name.lexeme, var_type, var_name.line_id);
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
    table.PushScope();
    ParseBlock(scope);
    table.PopScope();
    return scope;
  }

  ASTNode ParseDecl() {
    VarType const &var_type = ExpectToken(Lexer::ID_TYPE);
    Token const &ident = ExpectToken(Lexer::ID_ID);
    if (IfToken(Lexer::ID_ENDLINE)) {
      table.AddVar(ident.lexeme, var_type, ident.line_id);
      return ASTNode{};
    }
    ExpectToken(Lexer::ID_ASSIGN);

    ASTNode expr = ParseExpr();
    ExpectToken(Lexer::ID_ENDLINE);

    // don't add until _after_ we possibly resolve idents in expression
    // ex. var foo = foo should error if foo is undefined
    size_t var_id = table.AddVar(ident.lexeme, var_type, ident.line_id);

    ASTNode out = ASTNode{ASTNode::ASSIGN};
    out.AddChildren(ASTNode(ASTNode::IDENTIFIER, var_id, &ident),
                    std::move(expr));

    return out;
  }

  ASTNode ParseExpr() { return ParseAssign(); }

  ASTNode ParseAssign() {
    ASTNode lhs = ParseOr();
    if (CurToken().lexeme == "=") {
      // can only have variable names as the LHS of an assignment
      if (lhs.type != ASTNode::IDENTIFIER) {
        ErrorUnexpected(CurToken(), Lexer::ID_ID);
      }
      ExpectToken(Lexer::ID_ASSIGN);
      ASTNode rhs = ParseAssign();
      return ASTNode(ASTNode::ASSIGN, "=", std::move(lhs), std::move(rhs));
    }
    return lhs;
  }

  ASTNode ParseOr() {
    auto lhs = std::make_unique<ASTNode>(ParseAnd());
    while (CurToken().lexeme == "||") {
      ConsumeToken();
      ASTNode rhs = ParseAnd();
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

      if (lhs->ReturnType(table) == VarType::CHAR ||
          rhs.ReturnType(table) == VarType::CHAR) {
        Error(CurToken(), "Invalid action: Cannot perform multiplication, "
                          "division, or modulus with a char type!");
      }

      if (operation == "%" && (lhs->ReturnType(table) == VarType::DOUBLE ||
                               rhs.ReturnType(table) == VarType::DOUBLE)) {
        Error(CurToken(),
              "Invalid action: Cannot perform modulus with a double type!");
      }

      lhs = std::make_unique<ASTNode>(ASTNode(ASTNode::OPERATION, operation,
                                              std::move(*lhs), std::move(rhs)));
    }
    return ASTNode{std::move(*lhs)};
  }

  ASTNode ParseNegate() {
    auto lhs = std::make_unique<ASTNode>(ASTNode::LITERAL, -1);
    Token const &curr_token = CurToken();
    auto rhs = ParseTerm();

    if (rhs.ReturnType(table) == VarType::CHAR) {
      Error(curr_token, "Invalid action: Cannot negate a char type!");
    }

    return ASTNode(ASTNode::OPERATION, "*", std::move(*lhs), std::move(rhs));
  }

  ASTNode ParseNOT() {
    Token const curr_token = CurToken();
    auto rhs = ParseTerm();

    if (rhs.ReturnType(table) != VarType::INT) {
      Error(curr_token, "Invalid action: Cannot perform a logical \"NOT\" on a "
                        "type thats not an INT!");
    }

    return ASTNode(ASTNode::OPERATION, "!", std::move(rhs));
  }

  ASTNode ParseSqrt() {
    auto inside = ParseExpr();
    return ASTNode(ASTNode::OPERATION, "sqrt", std::move(inside));
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
      ASTNode out{ASTNode::CAST_DOUBLE};
      out.AddChildren(std::move(node));
      return out;
    }

    Error(token, "Attempt to cast to unknown type ", token.lexeme.substr(1));
  }

  ASTNode ParseTerm() {
    Token const &current = CurToken();
    switch (current) {
    case Lexer::ID_FLOAT:
      return CheckTypeCast(
          ASTNode(ASTNode::LITERAL, std::stod(ConsumeToken().lexeme)));
    case Lexer::ID_INT:
      return CheckTypeCast(
          ASTNode(ASTNode::LITERAL, std::stoi(ConsumeToken().lexeme)));
    case Lexer::ID_CHAR:
      return CheckTypeCast(ASTNode(ASTNode::LITERAL, ConsumeToken().lexeme[1]));
    case Lexer::ID_ID:
      return CheckTypeCast(ASTNode(
          ASTNode::IDENTIFIER,
          table.FindVar(ConsumeToken().lexeme, current.line_id), &current));
    case Lexer::ID_OPEN_PARENTHESIS: {
      ExpectToken(Lexer::ID_OPEN_PARENTHESIS);
      ASTNode subexpression = ParseExpr();
      ExpectToken(Lexer::ID_CLOSE_PARENTHESIS);
      return CheckTypeCast(std::move(subexpression));
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

  WATExpr GenerateCode() {
    State state{std::move(table)};
    return root.EmitModule(state);
  }
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
