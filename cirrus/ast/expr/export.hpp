#pragma once

#include <vector>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct ExportExpressionData : public ExpressionData {
    Expression expression;
};

struct ExportExpression
    : public IExpression<ExportExpression, ExpressionVtbl, ExportExpressionData> {
    using IExpression<ExportExpression, ExpressionVtbl, ExportExpressionData>::IExpression;

    static const ExpressionVtbl _vtbl;

    [[nodiscard]] static bool is(const Expression expr) noexcept { return expr.vtbl() == &_vtbl; }
    [[nodiscard]] static ExportExpression alloc(Expression expression) noexcept;

    [[nodiscard]] constexpr const Expression& expression() const noexcept {
        return _data->expression;
    }
};

}  // namespace cirrus::ast
