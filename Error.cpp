#include "Error.hpp"

void ErrorUnsupportedUnary(Token const &token, Type const &type) {
  Error(token, "Operator ", token.lexeme, " not supported on values of type ",
        type.TypeName());
}

void ErrorUnsupportedBinary(Token const &token, Type const &lhs,
                            Type const &rhs) {
  Error(token, "Operator ", token.lexeme, " not supported between values of ",
        lhs.TypeName(), " and ", rhs.TypeName());
}
