#pragma once

#include <optional>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct ReturnExpressionData : public ExpressionData {
    std::optional<Expression> expr;
};

struct ReturnExpression
    : public IExpression<ReturnExpression, ExpressionVtbl, ReturnExpressionData> {
    using IExpression<ReturnExpression, ExpressionVtbl, ReturnExpressionData>::IExpression;

    static const ExpressionVtbl _vtbl;

    [[nodiscard]] static bool is(const Expression expr) noexcept { return expr.vtbl() == &_vtbl; }
    [[nodiscard]] static ReturnExpression alloc(const Expression expr) noexcept;

    [[nodiscard]] constexpr const std::optional<Expression>& expr() const noexcept {
        return _data->expr;
    }
};

}  // namespace cirrus::ast
