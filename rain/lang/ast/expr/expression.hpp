#pragma once

#include <cstdint>

#include "absl/base/nullability.h"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/serial/kind.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::ast {

class Expression {
  public:
    virtual ~Expression() = default;

    [[nodiscard]] virtual serial::ExpressionKind kind() const noexcept = 0;
    [[nodiscard]] virtual absl::Nullable<Type*>  type() const noexcept = 0;

    [[nodiscard]] virtual bool compile_time_capable() const noexcept { return false; }
    virtual util::Result<void> validate(Scope& scope) = 0;
};

}  // namespace rain::lang::ast
