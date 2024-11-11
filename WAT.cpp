#include "WAT.hpp"

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
  if (comment_queue.empty()) {
    out << Newline();
    return;
  }
  while (!comment_queue.empty()) {
    out << " ;; " << comment_queue.back();
    comment_queue.pop_back();
    out << Newline();
  }
}

void WATWriter::Write(WATExpr const &expr) {
  // write comment
  if (expr.comment && !expr.format.inline_comment) {
    out << ";; " << expr.comment.value() << Newline();
  }

  /// write atom
  out << "(" << expr.atom;

  curindent += INDENT;

  std::string separator = expr.format.inline_attrs ? " " : Newline();
  for (auto i = expr.attributes.cbegin(); i != expr.attributes.cend(); i++) {
    if (i != expr.attributes.cend()) {
      out << separator;
    }
    out << *i;
  }

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
    comment_queue.push_back(expr.comment.value());
  }
  if (expr.format.newline) {
    NewlineWithComments();
  }
}

std::string Quote(std::string in) { return '"' + in + '"'; }
