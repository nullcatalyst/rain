#include "rain/lang/ast/module.hpp"

namespace rain::lang::ast {

util::Result<void> Module::validate(Options& options) {
    for (auto& type : _scope.owned_types()) {
        auto result = type->resolve(options, _scope);
        FORWARD_ERROR(result);
    }

    for (auto& expression : _expressions) {
        auto result = expression->validate(options, _scope);
        FORWARD_ERROR(result);
    }
    return {};
}

}  // namespace rain::lang::ast
