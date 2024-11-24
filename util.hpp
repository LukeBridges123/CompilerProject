#pragma once
#include <sstream>
#include <string>

template <typename Container>
std::string join(Container container, std::string separator = " ",
                 bool prefix = false) {
  std::stringstream out;
  for (auto i = container.cbegin(); i != container.cend(); i++) {
    if (prefix || i != container.cbegin()) {
      out << separator;
    }
    out << *i;
  }
  return out.str();
}

template <typename T> std::string Quote(T in) { return '"' + std::string{in} + '"'; } 


template <typename... T> std::string Variable(T... components) {
  std::stringstream out{};
  out << "$";
  (out << ... << components);
  return out.str();
}
