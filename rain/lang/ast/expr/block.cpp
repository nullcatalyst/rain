#include "rain/lang/ast/expr/block.hpp"

namespace rain::lang::ast {

util::Result<void> BlockExpression::validate(Scope& scope) {
    for (auto& expr : _expressions) {
        auto result = expr->validate(_scope);
        FORWARD_ERROR(result);
    }

    if (!_expressions.empty()) {
        _type = _expressions.back()->type();
    }

    return {};
}

}  // namespace rain::lang::ast
