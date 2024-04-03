#include "rain/lang/ast/expr/null.hpp"

#include "rain/lang/ast/scope/builtin.hpp"

namespace rain::lang::ast {

util::Result<void> NullExpression::validate(Options& options, Scope& scope) {
    // null expressions are always valid.
    return {};
}

}  // namespace rain::lang::ast
