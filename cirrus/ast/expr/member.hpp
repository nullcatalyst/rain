#pragma once

#include "cirrus/ast/expr/expression.hpp"
#include "cirrus/util/string.hpp"

namespace cirrus::ast {

class MemberExpression : public Expression {
    ExpressionPtr _expression;
    util::String  _member;

  public:
    MemberExpression(ExpressionPtr expression, util::String member)
        : _expression(std::move(expression)), _member(member) {}

    [[nodiscard]] static std::unique_ptr<MemberExpression> alloc(ExpressionPtr expression,
                                                                 util::String  member) {
        return std::make_unique<MemberExpression>(std::move(expression), member);
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::MemberExpression;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        return _expression->compile_time_capable();
    }

    [[nodiscard]] const ExpressionPtr& expression() const noexcept { return _expression; }
    [[nodiscard]] util::String         member() const noexcept { return _member; }
};

}  // namespace cirrus::ast
