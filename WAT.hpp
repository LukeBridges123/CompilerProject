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
};

class WATWriter {
private:
  int curindent = 0;
  std::string Indent() const;

public:
  void Write(std::ostream &out, WATExpr const &expr);
};
