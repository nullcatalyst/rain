#pragma once

#include "rain/ast/expr/expression.hpp"
#include "rain/util/string.hpp"

namespace rain::ast {

class MemberExpression : public Expression {
    ExpressionPtr _expression;
    util::String  _member;

    lang::Location _dot_location;
    lang::Location _member_location;

  public:
    MemberExpression(ExpressionPtr expression, util::String member)
        : _expression(std::move(expression)), _member(member) {}
    MemberExpression(ExpressionPtr expression, util::String member,
                     const lang::Location dot_location, const lang::Location member_location)
        : Expression(expression->location().merge(_member_location)),
          _expression(std::move(expression)),
          _member(member),
          _dot_location(dot_location),
          _member_location(member_location) {}

    [[nodiscard]] static std::unique_ptr<MemberExpression> alloc(ExpressionPtr expression,
                                                                 util::String  member) {
        return std::make_unique<MemberExpression>(std::move(expression), member);
    }
    [[nodiscard]] static std::unique_ptr<MemberExpression> alloc(
        ExpressionPtr expression, util::String member, const lang::Location dot_location,
        const lang::Location member_location) {
        return std::make_unique<MemberExpression>(std::move(expression), member, dot_location,
                                                  member_location);
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

}  // namespace rain::ast
