#include "rain/lang/ast/var/block.hpp"

namespace rain::lang::ast {

util::Result<void> BlockVariable::validate(Options& options, Scope& scope) noexcept {
    auto resolved_type = _type->resolve(scope);
    FORWARD_ERROR(resolved_type);

    _type = std::move(resolved_type).value();
    return {};
}

}  // namespace rain::lang::ast
