#pragma once

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct MemberExpressionData : public ExpressionData {
    Expression       expr;
    std::string_view member;
};

struct MemberExpression
    : public IExpression<MemberExpression, ExpressionVtbl, MemberExpressionData> {
    using IExpression<MemberExpression, ExpressionVtbl, MemberExpressionData>::IExpression;

    static const ExpressionVtbl _vtbl;

    [[nodiscard]] static bool is(const Expression expr) noexcept { return expr.vtbl() == &_vtbl; }
    [[nodiscard]] static MemberExpression alloc(Expression expr, std::string_view member) noexcept;

    [[nodiscard]] constexpr const Expression& expr() const noexcept { return _data->expr; }
    [[nodiscard]] constexpr std::string_view  member() const noexcept { return _data->member; }
};

}  // namespace cirrus::ast
