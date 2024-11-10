#include <cassert>
#include <format>
#include <fstream>
#include <memory>
#include <string>

#include <vector>

#include "ASTNode.hpp"
#include "Error.hpp"
#include "SymbolTable.hpp"
#include "Type.hpp"
#include "WAT.hpp"
#include "lexer.hpp"

using namespace emplex;

class Tubular {
private:
  std::vector<Token> tokens{};
  emplex::Lexer lexer{};
  SymbolTable table{};
  size_t token_idx{0};
  ASTNode root{ASTNode::MODULE};

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
      Type var_type = ExpectToken(Lexer::ID_TYPE);
      Token var_name = ExpectToken(Lexer::ID_ID);

      size_t var_id =
          table.AddVar(var_name.lexeme, Type::DOUBLE, var_name.line_id);
      func_info.arguments.emplace_back(var_name.lexeme, var_id);
      IfToken(','); // consume comma if exists
    }
    ConsumeToken(); // close parenthesis

    // parse return type
    ExpectToken(':');
    func_info.rettype = ExpectToken(Lexer::ID_TYPE);

    // parse body
    ParseBlock(function);

    table.PopScope();
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
    ExpectToken(Lexer::ID_VAR);
    Token const &ident = ExpectToken(Lexer::ID_ID);
    if (IfToken(Lexer::ID_ENDLINE)) {
      table.AddVar(ident.lexeme, Type::DOUBLE, ident.line_id);
      return ASTNode{};
    }
    ExpectToken(Lexer::ID_ASSIGN);

    ASTNode expr = ParseExpr();
    ExpectToken(Lexer::ID_ENDLINE);

    // don't add until _after_ we possibly resolve idents in expression
    // ex. var foo = foo should error if foo is undefined
    size_t var_id = table.AddVar(ident.lexeme, Type::DOUBLE, ident.line_id);

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
    auto lhs = std::make_unique<ASTNode>(ParseExponentiation());
    while (CurToken().lexeme == "*" || CurToken().lexeme == "/" ||
           CurToken().lexeme == "%") {
      std::string operation = ConsumeToken().lexeme;
      ASTNode rhs = ParseExponentiation();
      lhs = std::make_unique<ASTNode>(ASTNode(ASTNode::OPERATION, operation,
                                              std::move(*lhs), std::move(rhs)));
    }
    return ASTNode{std::move(*lhs)};
  }

  ASTNode ParseExponentiation() {
    ASTNode lhs = ParseTerm();
    if (CurToken().lexeme == "**") {
      ConsumeToken();
      ASTNode rhs = ParseExponentiation();
      return ASTNode(ASTNode::OPERATION, "**", std::move(lhs), std::move(rhs));
    }
    return lhs;
  }

  ASTNode ParseNegate() {
    auto lhs = std::make_unique<ASTNode>(ASTNode::LITERAL, -1);
    auto rhs = ParseTerm();
    return ASTNode(ASTNode::OPERATION, "*", std::move(*lhs), std::move(rhs));
  }

  ASTNode ParseNOT() {
    auto rhs = ParseTerm();
    return ASTNode(ASTNode::OPERATION, "!", std::move(rhs));
  }

  ASTNode ParseSqrt(){
    auto inside = ParseExpr();
    return ASTNode(ASTNode::OPERATION, "sqrt", std::move(inside));
  }

  ASTNode ParseTerm() {
    Token const &current = CurToken();
    switch (current) {
    case Lexer::ID_FLOAT:
    case Lexer::ID_INT:
      return ASTNode(ASTNode::LITERAL, std::stod(ConsumeToken().lexeme));
    case Lexer::ID_ID:
      return ASTNode(ASTNode::IDENTIFIER,
                     table.FindVar(ConsumeToken().lexeme, current.line_id),
                     &current);
    case Lexer::ID_OPEN_PARENTHESIS: {
      ExpectToken(Lexer::ID_OPEN_PARENTHESIS);
      ASTNode subexpression = ParseExpr();
      ExpectToken(Lexer::ID_CLOSE_PARENTHESIS);
      return subexpression;
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
    case Lexer::ID_SQRT:
      ConsumeToken();
      return ParseSqrt();
    default:
      ErrorUnexpected(current);
    }
    return ASTNode{};
  }

  ASTNode ParseIF() {
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

    node.AddChild(ParseStatement());
    return node;
  }

  ASTNode ParseStatement() {
    Token const &current = CurToken();
    switch (current) {
    case Lexer::ID_FUNCTION:
      return ParseFunction();
    case Lexer::ID_SCOPE_START:
      return ParseScope();
    case Lexer::ID_VAR:
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
      return ParseIF();
    case Lexer::ID_WHILE:
      return ParseWhile();
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
      root.AddChild(ParseStatement());
    }
  }

  WATExpr GenerateCode() { return root.EmitModule(table); }
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

  WATWriter writer;
  writer.Write(std::cout, wat);
}
