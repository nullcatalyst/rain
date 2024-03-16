#pragma once

#include <cstdint>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"

namespace rain::lang::ast {

class IdentifierExpression : public Expression {
    std::string_view _name;

    absl::Nullable<Variable*> _variable = nullptr;

  public:
    IdentifierExpression(std::string_view name) : _name(name) {}
    ~IdentifierExpression() override = default;

    [[nodiscard]] constexpr std::string_view name() const noexcept { return _name; }

    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
