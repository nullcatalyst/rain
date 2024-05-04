#pragma once

#include <string_view>

#include "absl/base/nullability.h"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/ast/var/variable.hpp"

namespace rain::lang::ast {

class UnwrappedOptionalVariable : public Variable {
    absl::Nonnull<Variable*> _variable;

  public:
    UnwrappedOptionalVariable(absl::Nonnull<Variable*> variable) : _variable(variable) {
        assert(_variable != nullptr);
    }
    ~UnwrappedOptionalVariable() override = default;

    [[nodiscard]] serial::VariableKind kind() const noexcept override {
        return serial::VariableKind::UnwrappedOptional;
    }
    [[nodiscard]] std::string_view     name() const noexcept override { return _variable->name(); }
    [[nodiscard]] absl::Nonnull<Type*> type() const noexcept override {
        return &optional_type()->type();
    }
    [[nodiscard]] constexpr bool mutable_() const noexcept override { return false; }
    [[nodiscard]] lex::Location  location() const noexcept override { return lex::Location(); }

    [[nodiscard]] absl::Nonnull<Variable*>     variable() const noexcept { return _variable; }
    [[nodiscard]] absl::Nonnull<OptionalType*> optional_type() const noexcept {
        return static_cast<OptionalType*>(_variable->type());
    }

    [[nodiscard]] util::Result<void> validate(Options& options, Scope& scope) noexcept override;
};

}  // namespace rain::lang::ast
