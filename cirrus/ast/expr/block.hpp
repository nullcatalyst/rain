#pragma once

#include <vector>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct BlockExpressionData : public ExpressionData {
    std::vector<Expression> expressions;
};

struct BlockExpression : public IExpression<BlockExpression, ExpressionVtbl, BlockExpressionData> {
    using IExpression<BlockExpression, ExpressionVtbl, BlockExpressionData>::IExpression;

    static const ExpressionVtbl _vtbl;

    [[nodiscard]] static bool is(const Expression expr) noexcept { return expr.vtbl() == &_vtbl; }
    [[nodiscard]] static BlockExpression alloc(const std::vector<Expression> expressions) noexcept;

    [[nodiscard]] constexpr const std::vector<Expression>& expressions() const noexcept {
        return _data->expressions;
    }
};

}  // namespace cirrus::ast
