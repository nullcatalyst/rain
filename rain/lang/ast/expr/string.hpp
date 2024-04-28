#pragma once

#include <cstdint>
#include <string>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/type/type.hpp"

namespace rain::lang::ast {

class StringExpression : public Expression {
    std::string _value;

    absl::Nullable<Type*> _type = nullptr;
    lex::Location         _location;

  public:
    StringExpression(std::string value, lex::Location location)
        : _value(std::move(value)), _location(location) {}
    ~StringExpression() override = default;

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::String;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }

    [[nodiscard]] constexpr bool is_compile_time_capable() const noexcept override { return true; }

    // StringExpression
    [[nodiscard]] constexpr std::string_view value() const noexcept { return _value; }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
