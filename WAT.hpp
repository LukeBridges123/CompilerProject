#pragma once
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

constexpr int INDENT = 2;

struct FormatOptions {
  // add a newline after expression
  bool newline = false;
  // whether to write this expression inline with its parent
  bool write_inline = false;
  // put attrs on same line as atom, instead of on separate lines
  bool inline_attrs = true;
  // put comment on same line instead of preceding line
  bool inline_comment = true;
};

// separate vectors for arguments + children possibly isn't the best
// representation, but it makes formatting easier. might be an issue if we need
// to write an S-expr where child precedes argument, ex: (atom child attr)
struct WATExpr {
  std::string atom;
  std::vector<std::string> attributes{};
  std::vector<WATExpr> children{};
  std::optional<std::string> comment = std::nullopt;
  FormatOptions format{};

  // equivalent to aggregate constructor
  WATExpr(std::string atom, std::vector<std::string> attributes = {},
          std::vector<WATExpr> children = {},
          std::optional<std::string> comment = std::nullopt,
          FormatOptions format = {})
      : atom(atom), attributes(attributes), children(children),
        comment(comment), format(format) {};

  // expand attr strings into attributes vector
  template <typename... Args>
  WATExpr(std::string atom, Args &&...attrs) : atom(atom) {
    Attributes(std::forward<Args>(attrs)...);
  }

  template <typename T> void Attributes(T attr) { attributes.push_back(attr); }

  template <typename T, typename... Rest>
  void Attributes(T attr, Rest &&...rest) {
    attributes.push_back(attr);
    Attributes(rest...);
  }

  // from std::vector::emplace_back
  template <typename... Args> WATExpr &Child(Args &&...args) {
    children.push_back(WATExpr(std::forward<Args>(args)...));
    return children.back();
  };

  operator std::vector<WATExpr>() const { return {*this}; }

  void AddChildren(std::vector<WATExpr> new_children);
  WATExpr &Inline();
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
