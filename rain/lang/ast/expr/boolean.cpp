#include "rain/lang/ast/expr/boolean.hpp"

#include "rain/lang/ast/scope/builtin.hpp"

namespace rain::lang::ast {

util::Result<void> BooleanExpression::validate(Scope& scope) {
    _type = scope.builtin()->bool_type();
    return {};
}

}  // namespace rain::lang::ast
