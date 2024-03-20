#pragma once

#include <cstdint>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"

namespace rain::lang::ast {

class CallExpression;

class IdentifierExpression : public Expression {
    std::string_view _name;

    absl::Nullable<Variable*> _variable = nullptr;
    absl::Nullable<Type*>     _type     = nullptr;

    friend class CallExpression;

  public:
    IdentifierExpression(std::string_view name) : _name(name) {}
    ~IdentifierExpression() override = default;

    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Variable;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] constexpr std::string_view      name() const noexcept { return _name; }
    [[nodiscard]] constexpr absl::Nullable<Variable*> variable() const noexcept {
        return _variable;
    }

    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
