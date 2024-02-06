#pragma once

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct IntegerExpressionData : public ExpressionData {
    uint64_t value;
};

DECLARE_EXPRESSION(Integer) {
    EXPRESSION_COMMON_IMPL(Integer);

    [[nodiscard]] static IntegerExpression alloc(const uint64_t value) noexcept;

    [[nodiscard]] constexpr uint64_t value() const noexcept { return _data->value; }
};

}  // namespace cirrus::ast
