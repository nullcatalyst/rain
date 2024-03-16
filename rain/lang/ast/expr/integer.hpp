#pragma once

#include <cstdint>

#include "rain/lang/ast/expr/expression.hpp"

namespace rain::lang::ast {

class IntegerExpression : public Expression {
    uint64_t _value;

  public:
    IntegerExpression(uint64_t value) : _value(value) {}
    ~IntegerExpression() override = default;

    [[nodiscard]] constexpr uint64_t value() const noexcept { return _value; }

    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
