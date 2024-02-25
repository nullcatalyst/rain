#pragma once

#include <vector>

#include "rain/ast/expr/expression.hpp"

namespace rain::ast {

class ExportExpression : public Expression {
    ExpressionPtr _expression;

  public:
    ExportExpression(ExpressionPtr expression) : _expression(std::move(expression)) {}

    [[nodiscard]] static std::unique_ptr<ExportExpression> alloc(ExpressionPtr expression) {
        return std::make_unique<ExportExpression>(std::move(expression));
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::ExportExpression;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override { return false; }

    [[nodiscard]] const ExpressionPtr& expression() const noexcept { return _expression; }
};

}  // namespace rain::ast
