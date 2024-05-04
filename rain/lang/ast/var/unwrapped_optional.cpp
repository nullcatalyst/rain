#include "rain/lang/ast/var/unwrapped_optional.hpp"

namespace rain::lang::ast {

util::Result<void> UnwrappedOptionalVariable::validate(Options& options, Scope& scope) noexcept {
    return _variable->validate(options, scope);
}

}  // namespace rain::lang::ast
