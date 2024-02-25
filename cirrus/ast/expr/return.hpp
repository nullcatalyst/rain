#pragma once

#include <optional>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

class ReturnExpression : public Expression {
    std::optional<ExpressionPtr> _value;

  public:
    ReturnExpression(std::optional<ExpressionPtr> value) : _value(std::move(value)) {}

    [[nodiscard]] static std::unique_ptr<ReturnExpression> alloc(
        std::optional<ExpressionPtr> value) {
        return std::make_unique<ReturnExpression>(std::move(value));
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::ReturnExpression;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        return !_value.has_value() || _value.value()->compile_time_capable();
    }

    [[nodiscard]] const std::optional<ExpressionPtr>& value() const noexcept { return _value; }
};

}  // namespace cirrus::ast
