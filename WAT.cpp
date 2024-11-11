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

  for (auto i = expr.attributes.cbegin(); i != expr.attributes.cend(); i++) {
    if (i != expr.attributes.cend())
      out << " ";
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

std::string Quote(std::string in) { return '"' + in + '"'; }
