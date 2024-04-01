#pragma once

#include <string>

#include "absl/base/nullability.h"
#include "rain/lang/lex/location.hpp"
#include "rain/lang/options.hpp"
#include "rain/lang/serial/kind.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::ast {

class Scope;

class Type {
  public:
    static std::string type_name(absl::Nullable<Type*> type) noexcept {
        if (type == nullptr) {
            return "<null_type>";
        }
        return type->name();
    }

    virtual ~Type() = default;

    [[nodiscard]] virtual serial::TypeKind kind() const noexcept = 0;
    [[nodiscard]] virtual std::string      name() const noexcept { return "<error_type>"; }
    [[nodiscard]] virtual lex::Location    location() const noexcept = 0;

    /**
     * Resolve the type to a concrete (and common) type. Fully resolved types can be compared using
     * pointer equality.
     *
     * The instance used to call this method is no longer valid after calling this method. If the
     * instance was being stored, then it should be replaced with the returned value.
     */
    [[nodiscard]] virtual util::Result<absl::Nonnull<Type*>> resolve(Options& options,
                                                                     Scope&   scope) = 0;
};

}  // namespace rain::lang::ast
