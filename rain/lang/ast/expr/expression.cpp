#include "rain/lang/ast/expr/expression.hpp"

#include "rain/util/console.hpp"

namespace rain::lang::ast {

void Expression::replace_type(absl::Nonnull<Type*> old_type, absl::Nonnull<Type*> new_type) {
    util::panic("Expression::replace_type not implemented");
}

}  // namespace rain::lang::ast
