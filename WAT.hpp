#pragma once
#include <iostream>
#include <optional>
#include <string>
#include <vector>

constexpr int INDENT = 2;

// separate vectors for arguments + children possibly isn't the best
// representation, but it makes formatting easier. might be an issue if we need
// to write an S-expr where child precedes argument, ex: (atom child attr)
struct WATExpr {
  std::string atom;
  std::vector<std::string> attributes{};
  std::vector<WATExpr> children{};
  std::optional<std::string> comment = std::nullopt;

  // from std::vector::emplace_back
  template <typename... Args> void Child(Args &&...args) {
    children.push_back(WATExpr(std::forward<Args>(args)...));
  };

  WATExpr(std::string atom) : atom(atom) {};
  WATExpr(std::string atom, std::vector<std::string> attributes)
      : atom(atom), attributes(attributes) {};
  template <typename... T> WATExpr(std::string atom, T... attrs) : atom(atom) {
    attributes.push_back(attrs...);
  };
};

class WATWriter {
private:
  int curindent = 0;
  std::string Indent() const;

public:
  void Write(std::ostream &out, WATExpr const &expr);
};
