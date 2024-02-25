#pragma once

#include "rain/ast/expr/expression.hpp"

namespace rain::ast {

class ParenthesisExpression : public Expression {
    ExpressionPtr _expression;

  public:
    ParenthesisExpression(ExpressionPtr expression) : _expression(std::move(expression)) {}

    [[nodiscard]] static std::unique_ptr<ParenthesisExpression> alloc(ExpressionPtr expression) {
        return std::make_unique<ParenthesisExpression>(std::move(expression));
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::ParenthesisExpression;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        return _expression->compile_time_capable();
    }

    [[nodiscard]] const ExpressionPtr& expression() const noexcept { return _expression; }
};

}  // namespace rain::ast
