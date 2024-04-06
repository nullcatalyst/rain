#include "rain/lang/ast/type/interface.hpp"

namespace rain::lang::ast {

util::Result<absl::Nonnull<Type*>> InterfaceType::resolve(Options& options, Scope& scope) {
    for (auto& method : _methods) {
        auto result = method->validate(options, scope);
        FORWARD_ERROR(result);
    }

    return this;
}

}  // namespace rain::lang::ast
