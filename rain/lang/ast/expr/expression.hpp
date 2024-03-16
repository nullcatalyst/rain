#pragma once

#include <cstdint>

#include "absl/base/nullability.h"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::ast {

class Expression {
  protected:
    absl::Nullable<Type*> _type = nullptr;

  public:
    virtual ~Expression() = default;

    [[nodiscard]] virtual bool compile_time_capable() const noexcept { return false; }

    virtual util::Result<void> validate(Scope& scope) = 0;
};

}  // namespace rain::lang::ast
