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

    lex::Location _location;

    friend class CallExpression;

  public:
    IdentifierExpression(std::string_view name, lex::Location location)
        : _name(name), _location(location) {}
    ~IdentifierExpression() override = default;

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Variable;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }
    [[nodiscard]] constexpr bool          is_assignable() const noexcept override {
        return _variable != nullptr && _variable->mutable_();
    }

    // IdentifierExpression
    [[nodiscard]] constexpr std::string_view          name() const noexcept { return _name; }
    [[nodiscard]] constexpr absl::Nullable<Variable*> variable() const noexcept {
        return _variable;
    }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
