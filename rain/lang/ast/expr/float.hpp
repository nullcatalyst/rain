#pragma once

#include <cstdint>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/type/type.hpp"

namespace rain::lang::ast {

class FloatExpression : public Expression {
    double _value;

    absl::Nullable<Type*> _type = nullptr;

  public:
    FloatExpression(uint64_t value) : _value(value) {}
    ~FloatExpression() override = default;

    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Float;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] constexpr uint64_t              value() const noexcept { return _value; }

    [[nodiscard]] bool compile_time_capable() const noexcept override { return true; }
    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
