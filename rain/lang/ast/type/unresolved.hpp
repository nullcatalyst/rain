#pragma once

#include <string_view>

#include "absl/base/nullability.h"
#include "rain/lang/ast/type/type.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::ast {

/**
 * Represents a named type that has not yet been resolved.
 */
class UnresolvedType : public Type {
    std::string_view _name;

  public:
    UnresolvedType(std::string_view name) : _name(name) {}
    ~UnresolvedType() override = default;

    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Unresolved;
    }
    [[nodiscard]] constexpr std::string_view name() const noexcept { return _name; }

#if !defined(NDEBUG)
    [[nodiscard]] std::string debug_name() const noexcept override { return std::string(_name); }
#endif  // !defined(NDEBUG)

    [[nodiscard]] util::Result<absl::Nonnull<Type*>> resolve(Scope& scope) override;
};

}  // namespace rain::lang::ast
