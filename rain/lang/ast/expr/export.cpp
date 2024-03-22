#include "rain/lang/ast/expr/export.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/err/simple.hpp"

namespace rain::lang::ast {

util::Result<void> ExportExpression::validate(Scope& scope) {
    if (_expression->kind() != serial::ExpressionKind::Function) {
        return ERR_PTR(err::SimpleError, "only functions can be exported");
    }

    // TODO: Only allow exporting functions that do not have a callee.

    return _expression->validate(scope);
}

}  // namespace rain::lang::ast
