#pragma once

#include <vector>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct CallExpressionData : public ExpressionData {
    Expression              callee;
    std::vector<Expression> arguments;
};

DECLARE_EXPRESSION(Call) {
    EXPRESSION_COMMON_IMPL(Call);

    [[nodiscard]] static CallExpression alloc(Expression              callee,
                                              std::vector<Expression> expressions) noexcept;

    [[nodiscard]] constexpr const Expression& callee() const noexcept { return _data->callee; }
    [[nodiscard]] constexpr const std::vector<Expression>& arguments() const noexcept {
        return _data->arguments;
    }
};

}  // namespace cirrus::ast
