#pragma once

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

class IntegerExpression : public Expression {
    uint64_t _value;

  public:
    IntegerExpression(const uint64_t value) : _value(value) {}

    [[nodiscard]] static std::shared_ptr<IntegerExpression> alloc(const uint64_t value) {
        return std::make_shared<IntegerExpression>(value);
    }

    [[nodiscard]] NodeKind kind() const noexcept override { return NodeKind::IntegerExpression; }

    [[nodiscard]] constexpr bool compile_time_capable() const noexcept override { return true; }

    [[nodiscard]] uint64_t value() const noexcept { return _value; }
};

}  // namespace cirrus::ast
