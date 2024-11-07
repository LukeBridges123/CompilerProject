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

  /// write attributes
  // s-expr with attributes can be written on one line,
  // but if s-expr has children then attrs and children each get own line
  std::string separator = expr.children.empty() ? " " : ("\n" + Indent());
  for (auto i = expr.attributes.cbegin(); i != expr.attributes.cend(); i++) {
    if (i != expr.attributes.cend())
      out << separator;
    out << *i;
  }

  /// write children
  for (WATExpr const &child : expr.children) {
    out << std::endl;
    Write(out, child);
  }
  curindent -= INDENT;

  out << ")";
}
