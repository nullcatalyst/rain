#pragma once

#include <memory>
#include <string_view>

#include "absl/base/nullability.h"
#include "rain/lang/ast/type/type.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::ast {

class Scope;

class Variable {
  public:
    virtual ~Variable() = default;

    [[nodiscard]] virtual std::string_view     name() const noexcept     = 0;
    [[nodiscard]] virtual absl::Nonnull<Type*> type() const noexcept     = 0;
    [[nodiscard]] virtual bool                 mutable_() const noexcept = 0;

    [[nodiscard]] virtual util::Result<void> validate(Scope& scope) noexcept = 0;
};

}  // namespace rain::lang::ast

#include "rain/lang/ast/scope/scope.hpp"
