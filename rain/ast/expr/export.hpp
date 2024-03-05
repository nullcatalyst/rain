#pragma once

#include <vector>

#include "rain/ast/expr/expression.hpp"

namespace rain::ast {

class ExportExpression : public Expression {
    ExpressionPtr _expression;

    lang::Location _export_location;

  public:
    ExportExpression(ExpressionPtr expression) : _expression(std::move(expression)) {}
    ExportExpression(ExpressionPtr expression, const lang::Location export_location)
        : Expression(export_location.merge(expression->location())),
          _expression(std::move(expression)) {}

    [[nodiscard]] static std::unique_ptr<ExportExpression> alloc(ExpressionPtr expression) {
        return std::make_unique<ExportExpression>(std::move(expression));
    }
    [[nodiscard]] static std::unique_ptr<ExportExpression> alloc(
        ExpressionPtr expression, const lang::Location export_location) {
        return std::make_unique<ExportExpression>(std::move(expression), export_location);
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::ExportExpression;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override { return false; }

    [[nodiscard]] const ExpressionPtr& expression() const noexcept { return _expression; }
};

}  // namespace rain::ast
