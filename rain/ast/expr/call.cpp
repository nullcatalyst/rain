#include "rain/ast/expr/call.hpp"

namespace rain::ast {

[[nodiscard]] bool CallExpression::compile_time_capable() const noexcept {
    if (!_callee->compile_time_capable()) {
        return false;
    }
    for (const auto& arg : _arguments) {
        if (!arg->compile_time_capable()) {
            return false;
        }
    }
    return true;
}

}  // namespace rain::ast
