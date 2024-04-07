#include "rain/lang/ast/type/meta.hpp"

#include "rain/util/console.hpp"

namespace rain::lang::ast {

util::Result<absl::Nonnull<Type*>> MetaType::resolve(Options& options, Scope& scope) {
    return this;
}

}  // namespace rain::lang::ast
