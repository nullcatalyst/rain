#include "rain/lang/ast/expr/module.hpp"

namespace rain::lang::ast {

util::Result<void> Module::validate(Options& options) {
    for (auto& expression : _expressions) {
        auto result = expression->validate(options, _scope);
        FORWARD_ERROR(result);
    }
    return {};
}

}  // namespace rain::lang::ast
