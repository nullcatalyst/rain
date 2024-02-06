#include "cirrus/ast/expr/exec.hpp"

namespace cirrus::ast {

const ExpressionVtbl ExecExpression::_vtbl{
    EXPRESSION_VTBL_COMMON_IMPL(Exec),
    .compile_time_able =
        [](const ExpressionVtbl* const vtbl, ExpressionData* const data) noexcept {
            const ExecExpressionData* _data = static_cast<ExecExpressionData*>(data);
            return _data->expression.compile_time_able();
        },
};

ExecExpression ExecExpression::alloc(Expression expression) noexcept {
    ExecExpressionData* data = new ExecExpressionData{};
    data->expression         = std::move(expression);
    return ExecExpression::from_raw(&ExecExpression::_vtbl, data);
}

}  // namespace cirrus::ast
