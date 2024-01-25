#pragma once

#include <optional>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct IfExpressionData : public ExpressionData {
    Expression                condition;
    Expression                then;
    std::optional<Expression> else_;
};

struct IfExpression : public IExpression<IfExpression, ExpressionVtbl, IfExpressionData> {
    using IExpression<IfExpression, ExpressionVtbl, IfExpressionData>::IExpression;

    static const ExpressionVtbl _vtbl;

    [[nodiscard]] static bool is(const Expression expr) noexcept { return expr.vtbl() == &_vtbl; }
    [[nodiscard]] static IfExpression alloc(Expression condition, Expression then) noexcept;
    [[nodiscard]] static IfExpression alloc(Expression condition, Expression then,
                                            Expression else_) noexcept;
    [[nodiscard]] static IfExpression alloc(Expression condition, Expression then,
                                            std::optional<Expression> else_) noexcept;

    [[nodiscard]] constexpr const Expression& condition() const noexcept {
        return _data->condition;
    }
    [[nodiscard]] constexpr const Expression& then() const noexcept { return _data->then; }
    [[nodiscard]] constexpr const std::optional<Expression>& else_() const noexcept {
        return _data->else_;
    }
};

}  // namespace cirrus::ast
