#include "rain/lang/ast/module.hpp"

namespace rain::lang::ast {

util::Result<void> Module::validate(Options& options) {
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

    return {};
}

}  // namespace rain::lang::ast
