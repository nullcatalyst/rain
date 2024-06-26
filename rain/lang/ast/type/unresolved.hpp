#pragma once

#include <string_view>

#include "absl/base/nullability.h"
#include "llvm/ADT/SmallVector.h"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/lex/location.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::ast {

/**
 * Represents a named type that has not yet been resolved.
 */
class UnresolvedType : public Type {
    std::string_view _name;

    lex::Location _location;

  public:
    UnresolvedType(std::string_view name, lex::Location location)
        : _name(name), _location(location) {}
    ~UnresolvedType() override = default;

    // Type
    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Unresolved;
    }
    [[nodiscard]] constexpr std::string display_name() const noexcept override {
        return std::string(_name);
    }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }

    [[nodiscard]] constexpr std::string_view name() const noexcept { return _name; }

  protected:
    [[nodiscard]] util::Result<absl::Nonnull<Type*>> _resolve(Options& options,
                                                              Scope&   scope) override;
};

}  // namespace rain::lang::ast
