#pragma once

#include <vector>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct ExecExpressionData : public ExpressionData {
    Expression expression;
};

struct ExecExpression : public IExpression<ExecExpression, ExpressionVtbl, ExecExpressionData> {
    using IExpression<ExecExpression, ExpressionVtbl, ExecExpressionData>::IExpression;

    static const ExpressionVtbl _vtbl;

    [[nodiscard]] static bool is(const Expression expr) noexcept { return expr.vtbl() == &_vtbl; }
    [[nodiscard]] static ExecExpression alloc(Expression expression) noexcept;

    [[nodiscard]] constexpr const Expression& expression() const noexcept {
        return _data->expression;
    }
};

}  // namespace cirrus::ast
