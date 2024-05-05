#include "rain/lang/ast/expr/string.hpp"

#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/err/syntax.hpp"

namespace rain::lang::ast {

util::Result<void> StringExpression::validate(Options& options, Scope& scope) {
    _type = &scope.builtin()->u8_type()->get_slice_type(scope);
    return {};
}

}  // namespace rain::lang::ast
