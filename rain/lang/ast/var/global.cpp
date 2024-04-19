#include "rain/lang/ast/var/global.hpp"

namespace rain::lang::ast {

util::Result<void> GlobalVariable::validate(Options& options, Scope& scope) noexcept {
    {
        auto result = _type->resolve(options, scope);
        FORWARD_ERROR(result);

        _type = std::move(result).value();
    }

    return {};
}

}  // namespace rain::lang::ast
