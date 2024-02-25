#pragma once

#include "rain/ast/expr/expression.hpp"

namespace rain::ast {

class FloatExpression : public Expression {
    double _value;

  public:
    FloatExpression(const double value) : _value(value) {}

    [[nodiscard]] static std::unique_ptr<FloatExpression> alloc(const double value) {
        return std::make_unique<FloatExpression>(value);
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::FloatExpression;
    }

    [[nodiscard]] constexpr bool compile_time_capable() const noexcept override { return true; }

    [[nodiscard]] double value() const noexcept { return _value; }
};

}  // namespace rain::ast
