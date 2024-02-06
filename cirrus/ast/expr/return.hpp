#pragma once

#include <optional>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct ReturnExpressionData : public ExpressionData {
    std::optional<Expression> expr;
};

DECLARE_EXPRESSION(Return) {
    EXPRESSION_COMMON_IMPL(Return);

    [[nodiscard]] static ReturnExpression alloc(const Expression expr) noexcept;

    [[nodiscard]] constexpr const std::optional<Expression>& expr() const noexcept {
        return _data->expr;
    }
};

}  // namespace cirrus::ast
