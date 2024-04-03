#pragma once

#include <cstdint>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/type/type.hpp"

namespace rain::lang::ast {

class NullExpression : public Expression {
    lex::Location _location;

  public:
    NullExpression(lex::Location location) : _location(location) {}
    ~NullExpression() override = default;

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Null;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return nullptr; }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }
    [[nodiscard]] bool compile_time_capable() const noexcept override { return true; }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
