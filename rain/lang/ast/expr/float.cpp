#include "rain/lang/ast/expr/float.hpp"

#include "rain/lang/ast/scope/builtin.hpp"

namespace rain::lang::ast {

util::Result<void> FloatExpression::validate(Scope& scope) {
    _type = scope.builtin()->f32_type();
    return {};
}

}  // namespace rain::lang::ast
