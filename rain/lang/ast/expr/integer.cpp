#include "rain/lang/ast/expr/integer.hpp"

#include "rain/lang/ast/scope/builtin.hpp"

namespace rain::lang::ast {

util::Result<void> IntegerExpression::validate(Scope& scope) {
    _type = scope.builtin()->i32_type();
    return {};
}

}  // namespace rain::lang::ast