#pragma once

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct ParenthesisExpressionData : public ExpressionData {
    Expression expr;
};

DECLARE_EXPRESSION(Parenthesis) {
    EXPRESSION_COMMON_IMPL(Parenthesis);

    [[nodiscard]] static ParenthesisExpression alloc(const Expression expr) noexcept;

    [[nodiscard]] constexpr const Expression& expr() const noexcept { return _data->expr; }
};

}  // namespace cirrus::ast
