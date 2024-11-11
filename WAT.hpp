#pragma once
#include <concepts>
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

  // equivalent to aggregate constructor
  WATExpr(std::string atom, std::vector<std::string> attributes = {},
          std::vector<WATExpr> children = {},
          std::optional<std::string> comment = std::nullopt)
      : atom(atom), attributes(attributes), children(children),
        comment(comment) {};

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

  void AddChildren(std::vector<WATExpr> new_children) {
    std::move(new_children.begin(), new_children.end(),
              std::back_inserter(children));
  };
};

class WATWriter {
private:
  int curindent = 0;
  std::string Indent() const;

public:
  void Write(std::ostream &out, WATExpr const &expr);
};

std::string Quote(std::string in);
