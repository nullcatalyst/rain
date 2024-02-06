#pragma once

#include <vector>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct BlockExpressionData : public ExpressionData {
    std::vector<Expression> expressions;
};

DECLARE_EXPRESSION(Block) {
    EXPRESSION_COMMON_IMPL(Block);

    [[nodiscard]] static BlockExpression alloc(const std::vector<Expression> expressions) noexcept;

    [[nodiscard]] constexpr const std::vector<Expression>& expressions() const noexcept {
        return _data->expressions;
    }
    [[nodiscard]] constexpr std::vector<Expression>& expressions() noexcept {
        return _data->expressions;
    }
};

}  // namespace cirrus::ast
