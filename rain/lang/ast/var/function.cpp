#include "rain/lang/ast/var/function.hpp"

namespace rain::lang::ast {

[[nodiscard]] util::Result<void> FunctionVariable::validate(Options& options,
                                                            Scope&   scope) noexcept {
    auto resolved_type = _function_type->resolve(options, scope);
    FORWARD_ERROR(resolved_type);

    _function_type = reinterpret_cast<FunctionType*>(std::move(resolved_type).value());
    return {};
}

}  // namespace rain::lang::ast
