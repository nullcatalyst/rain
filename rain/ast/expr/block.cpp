#include "rain/ast/expr/block.hpp"

namespace rain::ast {

[[nodiscard]] bool BlockExpression::compile_time_capable() const noexcept {
    for (const auto& expr : _expressions) {
        if (!expr->compile_time_capable()) {
            return false;
        }
    }
    return true;
}

}  // namespace rain::ast
