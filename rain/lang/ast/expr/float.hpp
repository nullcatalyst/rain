#pragma once

#include <cstdint>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/type/type.hpp"

namespace rain::lang::ast {

class FloatExpression : public Expression {
  public:
    static constexpr auto Kind = serial::ExpressionKind::Float;

  private:
    double _value;

    absl::Nullable<Type*> _type = nullptr;
    lex::Location         _location;

  public:
    FloatExpression(double value, lex::Location location) : _value(value), _location(location) {}
    ~FloatExpression() override = default;

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override { return Kind; }
    [[nodiscard]] constexpr absl::Nullable<Type*>  type() const noexcept override { return _type; }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }

    [[nodiscard]] constexpr bool is_compile_time_capable() const noexcept override { return true; }
    [[nodiscard]] constexpr bool is_constant() const noexcept override { return true; }

    // FloatExpression
    [[nodiscard]] constexpr double value() const noexcept { return _value; }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
