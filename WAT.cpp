#include "WAT.hpp"
#include "Error.hpp"
#include "util.hpp"
#include <limits>
#include <optional>
#include <ranges>
#include <variant>

// from https://en.cppreference.com/w/cpp/io/basic_istream/ignore
constexpr auto max_size = std::numeric_limits<std::streamsize>::max();

WATExpr &WATExpr::Push(WATExpr &child) {
  children.push_back(WATChild{std::in_place_type<WATExpr>, child});
  return *this;
}

WATExpr &WATExpr::Push(WATExpr &&child) {
  children.push_back(WATChild{std::in_place_type<WATExpr>, std::move(child)});
  return *this;
}

WATExpr &WATExpr::Push(std::vector<WATExpr> &&children) {
  std::ranges::move(children, std::back_inserter(this->children));
  return *this;
}

WATExpr &WATExpr::Inline() {
  format.write_inline = true;
  return *this;
}

WATExpr &WATExpr::Newline() {
  format.newline = true;
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

  bool write_attr_inline = expr.format.inline_attrs;
  for (size_t i = 0; i < expr.children.size(); i++) {
    WATChild const &child = expr.children.at(i);

    if (std::holds_alternative<std::string>(child)) {
      // write an attribute
      std::string separator = write_attr_inline ? " " : Newline();
      out << separator << std::get<std::string>(child);
    } else {
      // write a child expression
      WATExpr const &child_expr = std::get<WATExpr>(child);
      if (child_expr.format.write_inline) {
        out << " ";
      } else {
        NewlineWithComments();
      }
      Write(child_expr);
      // if child expr is not written inline, then stop writing attrs inline
      write_attr_inline &= child_expr.format.write_inline;
    }
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

WATParser::WATParser(unsigned char *array, size_t length) {
  // pubsetbuf trick from https://stackoverflow.com/a/7781958/4678913
  // rose: not sure if this reinterpret cast is legal but it works <3
  in.rdbuf()->pubsetbuf(reinterpret_cast<char *>(array), length);
}

// rose:
// quick and dirty parsing of WAT for internal functions
// this allows us to store our internal functions as WATExprs
// instead of just writing the text directly into the output
// stream like a normal person. because i think it's neat
// objectively a worse decision by every measureable metric
// but it does make the code to inject internal functions pretty :)
WATExpr WATParser::ParseExpr() {
  // lex atom
  std::string atom;
  while (!in.fail()) {
    switch (in.peek()) {
    case '(': // ignore leading paren
      in.get();
      continue;
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
      expr.Push(ParseExpr());
      break;
    // close paren means we're done
    case ')':
      if (!attr.empty())
        expr.children.push_back(std::string{attr});
      return expr;
    // ignore everything after comment until newline
    case ';':
      in.ignore(max_size, '\n');
      in.get();
      break;
    // if we have an attr and hit whitespace, attr is done
    case ' ':
    case '\n':
      if (!attr.empty()) {
        expr.children.push_back(std::string{attr});
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
  while (in >> token && !in.fail()) {
    // consume whitespace to make sure we haven't hit EOF
    if (std::isspace(token))
      continue;
    // ignore everything after comment until newline
    if (token == ';') {
      in.ignore(max_size, '\n');
      continue;
    }
    exprs.push_back(ParseExpr().Newline());
  }
  return exprs;
}
