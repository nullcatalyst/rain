#include "rain/ast/expr/block.hpp"

namespace rain::ast {

[[nodiscard]] bool BlockExpression::compile_time_capable() const noexcept {
    for (const auto& statement : _statements) {
        if (!statement->compile_time_capable()) {
            return false;
        }
    }

    return _expression != nullptr ? _expression->compile_time_capable() : true;
}

}  // namespace rain::ast
