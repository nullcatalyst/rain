#pragma once

#include <cstdint>

#include "absl/base/nullability.h"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/lex/location.hpp"
#include "rain/lang/options.hpp"
#include "rain/lang/serial/kind.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::ast {

class Expression {
  public:
    Expression()          = default;
    virtual ~Expression() = default;

    [[nodiscard]] virtual serial::ExpressionKind kind() const noexcept     = 0;
    [[nodiscard]] virtual absl::Nullable<Type*>  type() const noexcept     = 0;
    [[nodiscard]] virtual lex::Location          location() const noexcept = 0;

    [[nodiscard]] virtual bool compile_time_capable() const noexcept { return false; }
    [[nodiscard]] virtual bool is_assignable() const noexcept { return false; }

    [[nodiscard]] virtual util::Result<void> validate(Options& options, Scope& scope) = 0;
};

}  // namespace rain::lang::ast
