#pragma once

#include <optional>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct IfExpressionData : public ExpressionData {
    Expression                condition;
    Expression                then;
    std::optional<Expression> else_;
};

DECLARE_EXPRESSION(If) {
    EXPRESSION_COMMON_IMPL(If);

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
