#include "rain/lang/ast/expr/export.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/ast/expr/function.hpp"
#include "rain/lang/err/syntax.hpp"

namespace rain::lang::ast {

util::Result<void> ExportExpression::validate(Options& options, Scope& scope) {
    // if (_expression->kind() == serial::ExpressionKind::Method) {
    //     auto method = static_cast<MethodExpression*>(_expression.get());
    //     if (method->has_self_argument()) {
    //         return ERR_PTR(err::SyntaxError, _expression->location(),
    //                        "instance methods cannot (currently) be exported");
    //     }
    // } else
    if (_expression->kind() != serial::ExpressionKind::Function) {
        return ERR_PTR(err::SyntaxError, _expression->location(),
                       "only functions can (currently) be exported");
    }

    return _expression->validate(options, scope);
}

}  // namespace rain::lang::ast
