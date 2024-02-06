#include "cirrus/ast/expr/return.hpp"

namespace cirrus::ast {

const ExpressionVtbl ReturnExpression::_vtbl{
    EXPRESSION_VTBL_COMMON_IMPL(Return),
    .compile_time_able =
        [](const ExpressionVtbl* const vtbl, ExpressionData* const data) noexcept {
            const ReturnExpressionData* _data = static_cast<ReturnExpressionData*>(data);
            return !_data->expr.has_value() || _data->expr.value().compile_time_able();
        },
};

ReturnExpression ReturnExpression::alloc(const Expression expr) noexcept {
    ReturnExpressionData* data = new ReturnExpressionData{};
    data->expr                 = expr;
    return ReturnExpression::from_raw(&ReturnExpression::_vtbl, data);
}

}  // namespace cirrus::ast
