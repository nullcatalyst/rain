#include "rain/lang/ast/type/meta.hpp"

namespace rain::lang::ast {

util::Result<absl::Nonnull<Type*>> MetaType::resolve(Scope& scope) { return this; }

}  // namespace rain::lang::ast
