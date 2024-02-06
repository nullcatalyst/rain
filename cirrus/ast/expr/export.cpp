#include "cirrus/ast/expr/export.hpp"

namespace cirrus::ast {

const ExpressionVtbl ExportExpression::_vtbl{
    EXPRESSION_VTBL_COMMON_IMPL(Export),
    .compile_time_able =
        [](const ExpressionVtbl* const vtbl, ExpressionData* const data) noexcept {
            const ExportExpressionData* _data = static_cast<ExportExpressionData*>(data);
            return _data->expression.compile_time_able();
        },
};

ExportExpression ExportExpression::alloc(Expression expression) noexcept {
    ExportExpressionData* data = new ExportExpressionData{};
    data->expression           = std::move(expression);
    return ExportExpression::from_raw(&ExportExpression::_vtbl, data);
}

}  // namespace cirrus::ast
