#pragma once

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct MemberExpressionData : public ExpressionData {
    Expression       expr;
    std::string_view member;
};

DECLARE_EXPRESSION(Member) {
    EXPRESSION_COMMON_IMPL(Member);

    [[nodiscard]] static MemberExpression alloc(Expression expr, std::string_view member) noexcept;

    [[nodiscard]] constexpr const Expression& expr() const noexcept { return _data->expr; }
    [[nodiscard]] constexpr std::string_view  member() const noexcept { return _data->member; }
};

}  // namespace cirrus::ast
