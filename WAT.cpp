#include "WAT.hpp"

std::string WATWriter::Indent() const { return std::string(curindent, ' '); }

void WATWriter::Write(std::ostream &out, WATExpr const &expr) {
  // write comment
  if (expr.comment) {
    out << Indent() << ";; " << expr.comment.value() << std::endl;
  }

  /// write atom
  out << Indent() << "(" << expr.atom;

  curindent += INDENT;

  std::string separator = expr.format.inline_attrs ? " " : ("\n" + Indent());
  for (auto i = expr.attributes.cbegin(); i != expr.attributes.cend(); i++) {
    if (i != expr.attributes.cend())
      out << separator;
    out << *i;
  }

  /// write children
  for (size_t i = 0; i < expr.children.size(); i++) {
    WATExpr const &child = expr.children.at(i);
    if (i >= child.format.inline_children) {
      out << std::endl;
    } else {
      out << " ";
    }
    Write(out, child);
  }
  curindent -= INDENT;

  out << ")";
  if (expr.format.newline) {
    out << std::endl;
  }
}

std::string Quote(std::string in) { return '"' + in + '"'; }
