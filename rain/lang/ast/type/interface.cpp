#include "rain/lang/ast/type/interface.hpp"

namespace rain::lang::ast {

util::Result<absl::Nonnull<Type*>> InterfaceType::resolve(Options& options, Scope& scope) {
    // We only ever need to resolve the interface once. But if we don't check for that, then any
    // interface methods that take a self parameter will cause the interface type to attempt to
    // re-resolve itself, resulting in an endless loop. Prevent that here.
    if (_resolved) {
        return this;
    }
    _resolved = true;

    for (auto& method : _methods) {
        auto result = method->validate(options, scope);
        FORWARD_ERROR(result);
    }

    return this;
}

}  // namespace rain::lang::ast
