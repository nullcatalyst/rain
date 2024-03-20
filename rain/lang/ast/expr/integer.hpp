#pragma once

#include <cstdint>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/type/type.hpp"

namespace rain::lang::ast {

class IntegerExpression : public Expression {
    uint64_t _value;

    absl::Nullable<Type*> _type = nullptr;

  public:
    IntegerExpression(uint64_t value) : _value(value) {}
    ~IntegerExpression() override = default;

    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Integer;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] constexpr uint64_t              value() const noexcept { return _value; }

    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
