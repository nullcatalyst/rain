#pragma once

#include <string_view>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct IdentifierExpressionData : public ExpressionData {
    std::string_view name;
};

struct IdentifierExpression
    : public IExpression<IdentifierExpression, ExpressionVtbl, IdentifierExpressionData> {
    using IExpression<IdentifierExpression, ExpressionVtbl, IdentifierExpressionData>::IExpression;

    static const ExpressionVtbl _vtbl;

    [[nodiscard]] static bool is(const Expression expr) noexcept { return expr.vtbl() == &_vtbl; }
    [[nodiscard]] static IdentifierExpression alloc(const std::string_view name) noexcept;

    [[nodiscard]] constexpr std::string_view name() const noexcept { return _data->name; }
};

}  // namespace cirrus::ast
