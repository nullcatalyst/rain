#pragma once

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct IntegerExpressionData : public ExpressionData {
    uint64_t value;
};

struct IntegerExpression
    : public IExpression<IntegerExpression, ExpressionVtbl, IntegerExpressionData> {
    using IExpression<IntegerExpression, ExpressionVtbl, IntegerExpressionData>::IExpression;

    static const ExpressionVtbl _vtbl;

    [[nodiscard]] static bool is(const Expression expr) noexcept { return expr.vtbl() == &_vtbl; }
    [[nodiscard]] static IntegerExpression alloc(const uint64_t value) noexcept;

    [[nodiscard]] constexpr uint64_t value() const noexcept { return _data->value; }
};

}  // namespace cirrus::ast
