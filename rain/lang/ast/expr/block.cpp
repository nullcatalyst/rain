#include "rain/lang/ast/expr/block.hpp"

#include <algorithm>

namespace rain::lang::ast {

bool BlockExpression::compile_time_capable() const noexcept {
    return std::all_of(_expressions.begin(), _expressions.end(),
                       [](const auto& expression) { return expression->compile_time_capable(); });
}

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
