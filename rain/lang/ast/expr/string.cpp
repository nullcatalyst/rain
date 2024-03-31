#include "rain/lang/ast/expr/string.hpp"

#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/err/syntax.hpp"

namespace rain::lang::ast {

util::Result<void> StringExpression::validate(Scope& scope) {
    return ERR_PTR(err::SyntaxError, _location, "string expressions are not yet supported");
}

}  // namespace rain::lang::ast
