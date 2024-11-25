#pragma once
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

constexpr int INDENT = 2;

struct WATExpr; // forward declare so we can use definition in WATChild
using WATChild = std::variant<std::string, WATExpr>;

std::vector<WATChild> exprs_to_children(std::vector<WATExpr> &&exprs);

struct FormatOptions {
  // add a newline after expression
  bool newline = false;
  // whether to write this expression inline with its parent
  bool write_inline = false;
  // put attrs on same line as atom, instead of on separate lines
  // only applicable for attributes which precede child expressions
  bool inline_attrs = true;
  // put comment on same line instead of preceding line
  bool inline_comment = true;
};

struct WATExpr {
  std::string atom;
  std::vector<WATChild> children{};
  std::optional<std::string> comment = std::nullopt;
  FormatOptions format{};

  // helper constructor for using Expr children
  // WATExpr(std::string atom, std::vector<WATExpr> children)
  //     : atom(atom), children(exprs_to_children(std::move(children))) {};

  // expand children args into children vector
  template <typename... Args>
  WATExpr(std::string atom, Args &&...children) : atom(atom) {
    if constexpr (sizeof...(children) > 0) {
      Push(std::forward<Args>(children)...);
    }
  }

  template <typename T, typename... Rest> void Push(T &&child, Rest &&...rest) {
    Push(child);
    Push(std::forward<Rest>(rest)...);
  }

  void Push(WATExpr &&child) {
    children.push_back(WATChild{std::in_place_type<WATExpr>, std::move(child)});
  }

  void Push(std::vector<WATExpr> &&children) {
    AddChildren(std::move(children));
  }

  template <typename T> void Push(T &&child) {
    children.push_back(
        WATChild{std::in_place_type<std::string>, std::string{child}});
  }

  // from std::vector::emplace_back
  template <typename... Args> WATExpr &Child(Args &&...args) {
    children.push_back(WATExpr(std::forward<Args>(args)...));
    return std::get<WATExpr>(children.back());
  };

  operator std::vector<WATExpr>() const { return {*this}; }

  void AddChildren(std::vector<WATChild> new_children);
  void AddChildren(std::vector<WATExpr> &&new_children);
  WATExpr &Inline();
  WATExpr &Newline();
  WATExpr &Comment(std::string comment, bool inline_comment = true);
};

class WATWriter {
private:
  int curindent = 0;
  std::ostream &out;
  // buffer for inline comments, since we can't write them out
  // until after we've finished writing out close parens for a line,
  // and we could end up with multiple inline comments
  // intended for the same line
  std::vector<std::string> comment_queue{};

  std::string Indent() const;
  std::string Newline() const;
  void NewlineWithComments();

public:
  WATWriter(std::ostream &out) : out(out) {};
  void Write(WATExpr const &expr);
};

class WATParser {
private:
  std::istringstream in;

public:
  WATParser(unsigned char *array, size_t length);
  WATExpr ParseExpr();
  std::vector<WATExpr> Parse();
};
