#pragma once

#include <vector>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct ExportExpressionData : public ExpressionData {
    Expression expression;
};

DECLARE_EXPRESSION(Export) {
    EXPRESSION_COMMON_IMPL(Export);

    [[nodiscard]] static ExportExpression alloc(Expression expression) noexcept;

    [[nodiscard]] constexpr const Expression& expression() const noexcept {
        return _data->expression;
    }
};

}  // namespace cirrus::ast
