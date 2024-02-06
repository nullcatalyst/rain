#include "cirrus/ast/expr/if.hpp"

namespace cirrus::ast {

const ExpressionVtbl IfExpression::_vtbl{
    EXPRESSION_VTBL_COMMON_IMPL(If),
    .compile_time_able =
        [](const ExpressionVtbl* const vtbl, ExpressionData* const data) noexcept {
            const IfExpressionData* _data = static_cast<IfExpressionData*>(data);
            return _data->condition.compile_time_able() && _data->then.compile_time_able() &&
                   (!_data->else_.has_value() || _data->else_.value().compile_time_able());
        },
};

IfExpression IfExpression::alloc(Expression condition, Expression then,
                                 std::optional<Expression> else_) noexcept {
    IfExpressionData* data = new IfExpressionData{};
    data->then             = std::move(then);
    data->condition        = std::move(condition);
    data->else_            = std::move(else_);
    return IfExpression::from_raw(&IfExpression::_vtbl, data);
}

}  // namespace cirrus::ast
