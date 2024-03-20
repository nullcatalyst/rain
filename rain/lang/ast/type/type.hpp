#pragma once

#include <string>

#include "absl/base/nullability.h"
#include "rain/lang/serial/kind.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::ast {

class Scope;

class Type {
  public:
    virtual ~Type() = default;

    [[nodiscard]] virtual serial::TypeKind kind() const noexcept = 0;

#if !defined(NDEBUG)
    [[nodiscard]] virtual std::string debug_name() const noexcept { return "<unknown>"; }
#endif  // !defined(NDEBUG)

    /**
     * Resolve the type to a concrete (and common) type. Fully resolved types can be compared using
     * pointer equality.
     *
     * The instance used to call this method is no longer valid after calling this method. If the
     * instance was being stored, then it should be replaced with the returned value.
     */
    [[nodiscard]] virtual util::Result<absl::Nonnull<Type*>> resolve(Scope& scope) = 0;
};

}  // namespace rain::lang::ast
