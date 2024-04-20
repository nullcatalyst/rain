#include "rain/lang/ast/expr/compile_time.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/err/simple.hpp"

namespace rain::lang::ast {

bool CompileTimeExpression::compile_time_capable() const noexcept {
    if (_expression->kind() == serial::ExpressionKind::CompileTime) {
        // There is no need to try to evaluate nested compile-time expressions at compile-time,
        // as the innermost one will be evaluated at compile-time, all the outer ones will be
        // too automatically.
        return false;
    }
    return _expression->compile_time_capable();
}

util::Result<void> CompileTimeExpression::validate(Options& options, Scope& scope) {
    return _expression->validate(options, scope);
}

}  // namespace rain::lang::ast
