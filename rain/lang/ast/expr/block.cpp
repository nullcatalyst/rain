#include "rain/lang/ast/expr/block.hpp"

#include <algorithm>

namespace rain::lang::ast {

bool BlockExpression::compile_time_capable() const noexcept {
    return std::all_of(_expressions.begin(), _expressions.end(),
                       [](const auto& expression) { return expression->compile_time_capable(); });
}

util::Result<void> BlockExpression::validate(Options& options, Scope& scope) {
    {
        auto result = _scope.validate(options);
        FORWARD_ERROR(result);
    }

    for (auto& expression : _expressions) {
        auto result = expression->validate(options, _scope);
        FORWARD_ERROR(result);
    }

    {
        auto result = _scope.cleanup();
        FORWARD_ERROR(result);
    }

    if (!_expressions.empty()) {
        _type = _expressions.back()->type();
    }

    return {};
}

}  // namespace rain::lang::ast
