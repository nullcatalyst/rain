#pragma once

#include "cirrus/ast/expr/expression.hpp"
#include "cirrus/util/twine.hpp"

namespace cirrus::ast {

struct MemberExpressionData : public ExpressionData {
    Expression  expr;
    util::Twine member;
};

DECLARE_EXPRESSION(Member) {
    EXPRESSION_COMMON_IMPL(Member);

    [[nodiscard]] static MemberExpression alloc(Expression expr, util::Twine member) noexcept;

    [[nodiscard]] constexpr const Expression&  expr() const noexcept { return _data->expr; }
    [[nodiscard]] constexpr const util::Twine& member() const noexcept { return _data->member; }
};

}  // namespace cirrus::ast
