#pragma once

#include "rain/ast/expr/expression.hpp"

namespace rain::ast {

class IntegerExpression : public Expression {
    uint64_t _value;

  public:
    IntegerExpression(const uint64_t value) : _value(value) {}
    IntegerExpression(const uint64_t value, const lang::Location location)
        : Expression(location), _value(value) {}

    [[nodiscard]] static std::unique_ptr<IntegerExpression> alloc(const uint64_t value) {
        return std::make_unique<IntegerExpression>(value);
    }
    [[nodiscard]] static std::unique_ptr<IntegerExpression> alloc(const uint64_t       value,
                                                                  const lang::Location location) {
        return std::make_unique<IntegerExpression>(value, location);
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::IntegerExpression;
    }

    [[nodiscard]] constexpr bool compile_time_capable() const noexcept override { return true; }

    [[nodiscard]] uint64_t value() const noexcept { return _value; }
};

}  // namespace rain::ast
