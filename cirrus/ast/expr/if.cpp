#include "cirrus/ast/expr/if.hpp"

namespace cirrus::ast {

const ExpressionVtbl IfExpression::_vtbl{
    /* Node */ {
        /* util::RetainableVtbl */ {
            .retain  = util::_Retainable_vtbl.retain,
            .release = util::_Retainable_vtbl.release,
            .drop =
                [](const util::RetainableVtbl* const vtbl,
                   util::RetainableData* const       data) noexcept {
                    IfExpressionData* _data = static_cast<IfExpressionData*>(data);
                    delete _data;
                },
        },
        /*.kind = */ NodeKind::IfExpression,
    },
};

IfExpression IfExpression::alloc(Expression condition, Expression then) noexcept {
    IfExpressionData* data = new IfExpressionData{};
    data->then             = std::move(then);
    data->condition        = std::move(condition);
    return IfExpression::from_raw(&IfExpression::_vtbl, data);
}

IfExpression IfExpression::alloc(Expression condition, Expression then, Expression else_) noexcept {
    IfExpressionData* data = new IfExpressionData{};
    data->then             = std::move(then);
    data->condition        = std::move(condition);
    data->else_            = std::move(else_);
    return IfExpression::from_raw(&IfExpression::_vtbl, data);
}

IfExpression IfExpression::alloc(Expression condition, Expression then,
                                 std::optional<Expression> else_) noexcept {
    IfExpressionData* data = new IfExpressionData{};
    data->then             = std::move(then);
    data->condition        = std::move(condition);
    data->else_            = std::move(else_);
    return IfExpression::from_raw(&IfExpression::_vtbl, data);
}

}  // namespace cirrus::ast
