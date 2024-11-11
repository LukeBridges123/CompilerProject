#pragma once
#include <sstream>
#include <string>

template <typename Container>
std::string join(Container container, std::string separator = " ") {
  std::stringstream out;
  for (auto i = container.cbegin(); i != container.cend(); i++) {
    if (i != container.cbegin()) {
      out << separator;
    }
    out << *i;
  }
  return out.str();
}
