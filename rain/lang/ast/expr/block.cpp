#include "rain/lang/ast/expr/block.hpp"

namespace rain::lang::ast {

util::Result<void> BlockExpression::validate(Scope& scope) {
    for (auto& expr : _expressions) {
        auto result = expr->validate(scope);
        FORWARD_ERROR(result);
    }
    return {};
}

}  // namespace rain::lang::ast
