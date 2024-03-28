#pragma once

#include <cstdint>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/type/type.hpp"

namespace rain::lang::ast {

class BooleanExpression : public Expression {
    bool                  _value;
    absl::Nullable<Type*> _type = nullptr;
    lex::Location         _location;

  public:
    BooleanExpression(bool value, lex::Location location) : _value(value), _location(location) {}
    ~BooleanExpression() override = default;

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Integer;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }

    // BooleanExpression
    [[nodiscard]] constexpr bool value() const noexcept { return _value; }

    [[nodiscard]] bool compile_time_capable() const noexcept override { return true; }
    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
