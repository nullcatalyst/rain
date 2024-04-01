#include "rain/lang/ast/type/opaque.hpp"

namespace rain::lang::ast {

util::Result<absl::Nonnull<Type*>> OpaqueType::resolve(Options& options, Scope& scope) {
    return this;
}

}  // namespace rain::lang::ast