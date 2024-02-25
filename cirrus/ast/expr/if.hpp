#pragma once

#include <optional>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

class IfExpression : public Expression {
    ExpressionPtr                _condition;
    ExpressionPtr                _then;
    std::optional<ExpressionPtr> _else;

  public:
    IfExpression(ExpressionPtr condition, ExpressionPtr then, std::optional<ExpressionPtr> else_)
        : _condition(std::move(condition)), _then(std::move(then)), _else(std::move(else_)) {}

    [[nodiscard]] static std::unique_ptr<IfExpression> alloc(ExpressionPtr                condition,
                                                             ExpressionPtr                then,
                                                             std::optional<ExpressionPtr> else_) {
        return std::make_unique<IfExpression>(std::move(condition), std::move(then),
                                              std::move(else_));
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::IfExpression;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        if (!_else.has_value()) {
            // Currently, we don't support compile-time evaluation of if expressions without an else
            return false;
        }

        return _condition->compile_time_capable() && _then->compile_time_capable() &&
               _else.value()->compile_time_capable();
    }

    [[nodiscard]] const ExpressionPtr& condition() const noexcept { return _condition; }
    [[nodiscard]] const ExpressionPtr& then() const noexcept { return _then; }
    [[nodiscard]] const std::optional<ExpressionPtr>& else_() const noexcept { return _else; }
};

}  // namespace cirrus::ast
