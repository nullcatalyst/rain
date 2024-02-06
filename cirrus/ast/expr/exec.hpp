#pragma once

#include <vector>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct ExecExpressionData : public ExpressionData {
    Expression expression;
};

DECLARE_EXPRESSION(Exec) {
    EXPRESSION_COMMON_IMPL(Exec);

    [[nodiscard]] static ExecExpression alloc(Expression expression) noexcept;

    [[nodiscard]] constexpr const Expression& expression() const noexcept {
        return _data->expression;
    }
};

}  // namespace cirrus::ast
