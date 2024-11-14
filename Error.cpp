#include "Error.hpp"

void ErrorUnsupportedUnary(Token const &token, VarType const &type) {
  Error(token, "Operator ", token.lexeme, " not supported on values of type ",
        type.TypeName());
}

void ErrorUnsupportedBinary(Token const &token, VarType const &lhs,
                            VarType const &rhs) {
  Error(token, "Operator ", token.lexeme, " not supported between values of ",
        lhs.TypeName(), " and ", rhs.TypeName());
}
