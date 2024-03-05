#pragma once

#include "rain/ast/expr/expression.hpp"

namespace rain::ast {

class BooleanExpression : public Expression {
    bool _value;

  public:
    BooleanExpression(const bool value) : _value(value) {}
    BooleanExpression(const bool value, const lang::Location location)
        : Expression(location), _value(value) {}

    [[nodiscard]] static std::unique_ptr<BooleanExpression> alloc(const bool value) {
        return std::make_unique<BooleanExpression>(value);
    }
    [[nodiscard]] static std::unique_ptr<BooleanExpression> alloc(const bool           value,
                                                                  const lang::Location location) {
        return std::make_unique<BooleanExpression>(value, location);
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::BooleanExpression;
    }

    [[nodiscard]] constexpr bool compile_time_capable() const noexcept override { return true; }

    [[nodiscard]] bool value() const noexcept { return _value; }
};

}  // namespace rain::ast
