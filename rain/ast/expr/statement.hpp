#pragma once

#include <optional>

#include "rain/ast/expr/expression.hpp"

namespace rain::ast {

class StatementExpression : public Expression {
    ExpressionPtr _expression;

  public:
    StatementExpression(ExpressionPtr expression) : _expression(std::move(expression)) {}

    [[nodiscard]] static std::unique_ptr<StatementExpression> alloc(ExpressionPtr expression) {
        return std::make_unique<StatementExpression>(std::move(expression));
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::StatementExpression;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        return _expression->compile_time_capable();
    }

    [[nodiscard]] const ExpressionPtr& expression() const noexcept { return _expression; }
};

using StatementPtr = std::unique_ptr<StatementExpression>;

}  // namespace rain::ast
