#include "WAT.hpp"
#include "Error.hpp"
#include "util.hpp"
#include <limits>
#include <optional>

// from https://en.cppreference.com/w/cpp/io/basic_istream/ignore
constexpr auto max_size = std::numeric_limits<std::streamsize>::max();

void WATExpr::AddChildren(std::vector<WATExpr> new_children) {
  std::move(new_children.begin(), new_children.end(),
            std::back_inserter(children));
}

WATExpr &WATExpr::Inline() {
  format.write_inline = true;
  return *this;
}

WATExpr &WATExpr::Comment(std::string comment, bool inline_comment) {
  this->comment = comment;
  format.inline_comment = inline_comment;
  return *this;
}

std::string WATWriter::Indent() const { return std::string(curindent, ' '); }
std::string WATWriter::Newline() const { return "\n" + Indent(); }
void WATWriter::NewlineWithComments() {
  out << join(comment_queue, Newline());
  out << Newline();
  comment_queue.clear();
}

void WATWriter::Write(WATExpr const &expr) {
  // write comment
  if (expr.comment && !expr.format.inline_comment) {
    out << ";; " << expr.comment.value() << Newline();
  }

  /// write atom
  out << "(" << expr.atom;

  curindent += INDENT;

  // write attributes
  std::string separator = expr.format.inline_attrs ? " " : Newline();
  out << join(expr.attributes, separator, true);

  /// write children
  for (size_t i = 0; i < expr.children.size(); i++) {
    WATExpr const &child = expr.children.at(i);
    if (child.format.write_inline) {
      out << " ";
    } else {
      NewlineWithComments();
    }
    Write(child);
  }
  curindent -= INDENT;

  out << ")";
  if (expr.comment && expr.format.inline_comment) {
    comment_queue.push_back(" ;; " + expr.comment.value());
  }
  if (expr.format.newline) {
    NewlineWithComments();
  }
}

// quick and dirty parsing of WAT for internal functions
WATExpr WATParser::ParseExpr() {
  // lex atom
  std::string atom;
  while (!in.fail()) {
    switch (in.peek()) {
    case ')':
    case ' ':
    case '\n':
      break;
    default:
      atom += in.get();
      continue;
    }
    break;
  }

  WATExpr expr{atom};

  char token;
  std::string attr;
  while (in >> token) {
    switch (token) {
    // open paren means child
    case '(':
      expr.Child(ParseExpr());
      break;
    // close paren means we're done
    case ')':
      if (!attr.empty())
        expr.attributes.push_back(std::string{attr});
      return expr;
    // ignore everything until newline comments
    case ';':
      in.ignore(max_size, '\n');
      break;
    // if we have an attr and hit whitespace, attr is done
    case ' ':
    case '\n':
      if (!attr.empty()) {
        expr.attributes.push_back(std::string{attr});
        attr.clear();
      }
      break;
    // otherwise, this character is part of an attribute (or atom)
    default:
      attr += token;
    }
  }

  WATParseError("Unexpected EOF");
}

std::vector<WATExpr> WATParser::Parse() {
  std::vector<WATExpr> exprs;

  char token;
  in.unsetf(std::ios::skipws);
  while (in >> token) {
    // consume first paren
    if (token != '(') {
      WATParseError("Expected open parenthesis");
    }
    exprs.push_back(ParseExpr());
  }
  return exprs;
}
