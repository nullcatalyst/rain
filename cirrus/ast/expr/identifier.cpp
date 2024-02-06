#include "cirrus/ast/expr/identifier.hpp"

namespace cirrus::ast {

const ExpressionVtbl IdentifierExpression::_vtbl{
    EXPRESSION_VTBL_COMMON_IMPL(Identifier),
    .compile_time_able =
        [](const ExpressionVtbl* const vtbl, ExpressionData* const data) noexcept {
            // TODO: This must be determined based on the variable's declaration
            //   1. If the variable is not declared as mutable, using a compile-time able value,
            //      then this should return true. (trivial case)
            //   2. If the variable is declared as mutable, BUT assigned using a non-compile-time
            //      able value AND there is no mutation in between the declaration and the usage,
            //      then this should return true.
            //   3. If the variable is declared as mutable AND (assigned using a non-compile-time
            //      able value OR there is mutation in between the declaration and the usage), then
            //      this should return false.
            return true;
        },
};

IdentifierExpression IdentifierExpression::alloc(const std::string_view name) noexcept {
    IdentifierExpressionData* data = new IdentifierExpressionData{};
    data->name                     = name;
    return IdentifierExpression::from_raw(&IdentifierExpression::_vtbl, data);
}

}  // namespace cirrus::ast
