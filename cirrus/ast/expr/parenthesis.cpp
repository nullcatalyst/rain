#include "cirrus/ast/expr/parenthesis.hpp"

namespace cirrus::ast {

const ExpressionVtbl ParenthesisExpression::_vtbl{
    EXPRESSION_VTBL_COMMON_IMPL(Parenthesis),
    .compile_time_able =
        [](const ExpressionVtbl* const vtbl, ExpressionData* const data) noexcept {
            const ParenthesisExpressionData* _data = static_cast<ParenthesisExpressionData*>(data);
            return _data->expr.compile_time_able();
        },
};

ParenthesisExpression ParenthesisExpression::alloc(const Expression expr) noexcept {
    ParenthesisExpressionData* data = new ParenthesisExpressionData{};
    data->expr                      = expr;
    return ParenthesisExpression::from_raw(&ParenthesisExpression::_vtbl, data);
}

}  // namespace cirrus::ast
