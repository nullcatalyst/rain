#include "cirrus/ast/expr/binary_operator.hpp"

namespace cirrus::ast {

const ExpressionVtbl BinaryOperatorExpression::_vtbl{
    EXPRESSION_VTBL_COMMON_IMPL(BinaryOperator),
    .compile_time_able =
        [](const ExpressionVtbl* const vtbl, ExpressionData* const data) noexcept {
            const BinaryOperatorExpressionData* _data =
                static_cast<BinaryOperatorExpressionData*>(data);
            // TODO: This also depends on the implementation of the operator itself
            return _data->lhs.compile_time_able() && _data->rhs.compile_time_able();
        },
};

BinaryOperatorExpression BinaryOperatorExpression::alloc(Expression lhs, Expression rhs,
                                                         BinaryOperator op) noexcept {
    BinaryOperatorExpressionData* data = new BinaryOperatorExpressionData{};
    data->lhs                          = std::move(lhs);
    data->rhs                          = std::move(rhs);
    data->op                           = op;
    return BinaryOperatorExpression::from_raw(&BinaryOperatorExpression::_vtbl, data);
}

}  // namespace cirrus::ast
