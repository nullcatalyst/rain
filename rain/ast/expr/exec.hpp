#pragma once

#include <vector>

#include "rain/ast/expr/expression.hpp"

namespace rain::ast {

class ExecExpression : public Expression {
    ExpressionPtr _expression;

  public:
    ExecExpression(ExpressionPtr expression) : _expression(std::move(expression)) {}

    [[nodiscard]] static std::unique_ptr<ExecExpression> alloc(ExpressionPtr expression) {
        return std::make_unique<ExecExpression>(std::move(expression));
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::ExecExpression;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        return _expression->compile_time_capable();
    }

    [[nodiscard]] const ExpressionPtr& expression() const noexcept { return _expression; }
};

}  // namespace rain::ast
