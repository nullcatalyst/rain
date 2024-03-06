#pragma once

#include <vector>

#include "rain/ast/expr/expression.hpp"

namespace rain::ast {

class ExecExpression : public Expression {
    ExpressionPtr _expression;

    lang::Location _op_location;

  public:
    ExecExpression(ExpressionPtr expression) : _expression(std::move(expression)) {}
    ExecExpression(ExpressionPtr expression, const lang::Location op_location)
        : Expression(op_location.merge(expression->location())),
          _expression(std::move(expression)),
          _op_location(op_location) {}

    [[nodiscard]] static std::unique_ptr<ExecExpression> alloc(ExpressionPtr expression) {
        return std::make_unique<ExecExpression>(std::move(expression));
    }
    [[nodiscard]] static std::unique_ptr<ExecExpression> alloc(ExpressionPtr        expression,
                                                               const lang::Location op_location) {
        return std::make_unique<ExecExpression>(std::move(expression), op_location);
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::ExecExpression;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        return _expression->compile_time_capable();
    }

    [[nodiscard]] const ExpressionPtr&  expression() const noexcept { return _expression; }
    [[nodiscard]] const lang::Location& op_location() const noexcept { return _op_location; }
};

}  // namespace rain::ast
