#include "rain/lang/ast/expr/compile_time.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/err/simple.hpp"

namespace rain::lang::ast {

util::Result<void> CompileTimeExpression::validate(Options& options, Scope& scope) {
    return _expression->validate(options, scope);
}

}  // namespace rain::lang::ast
