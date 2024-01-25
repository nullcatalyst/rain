#pragma once

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct ParenthesisExpressionData : public ExpressionData {
    Expression expr;
};

struct ParenthesisExpression
    : public IExpression<ParenthesisExpression, ExpressionVtbl, ParenthesisExpressionData> {
    using IExpression<ParenthesisExpression, ExpressionVtbl,
                      ParenthesisExpressionData>::IExpression;

    static const ExpressionVtbl _vtbl;

    [[nodiscard]] static bool is(const Expression expr) noexcept { return expr.vtbl() == &_vtbl; }
    [[nodiscard]] static ParenthesisExpression alloc(const Expression expr) noexcept;

    [[nodiscard]] constexpr const Expression& expr() const noexcept { return _data->expr; }
};

}  // namespace cirrus::ast
