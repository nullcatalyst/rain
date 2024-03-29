#include "rain/lang/ast/expr/export.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/err/syntax.hpp"

namespace rain::lang::ast {

util::Result<void> ExportExpression::validate(Scope& scope) {
    if (_expression->kind() == serial::ExpressionKind::Method) {
        return ERR_PTR(err::SyntaxError, _expression->location(),
                       "methods cannot (currently) be exported");
    }

    if (_expression->kind() != serial::ExpressionKind::Function) {
        return ERR_PTR(err::SyntaxError, _expression->location(), "only functions can be exported");
    }

    return _expression->validate(scope);
}

}  // namespace rain::lang::ast
