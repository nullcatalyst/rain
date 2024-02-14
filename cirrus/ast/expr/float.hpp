#pragma once

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

class FloatExpression : public Expression {
    double _value;

  public:
    FloatExpression(const double value) : _value(value) {}

    [[nodiscard]] static std::shared_ptr<FloatExpression> alloc(const double value) {
        return std::make_shared<FloatExpression>(value);
    }

    [[nodiscard]] NodeKind kind() const noexcept override { return NodeKind::FloatExpression; }

    [[nodiscard]] constexpr bool compile_time_capable() const noexcept override { return true; }

    [[nodiscard]] double value() const noexcept { return _value; }
};

}  // namespace cirrus::ast
