#pragma once

#include "rain/lang/ast/type/type.hpp"

namespace rain::lang::ast {

/**
 * Represents an opaque type, where the implementation is hidden from the user of the language.
 *
 * Some examples of opaque types include the primitive types (`i32`, `f32`, `bool`, etc.) where
 * they cannot be defined within the language itself.
 */
class OpaqueType : public Type {
    std::string_view _name;

  public:
    OpaqueType(std::string_view name) : _name(name) {}
    ~OpaqueType() override = default;

#if !defined(NDEBUG)
    [[nodiscard]] std::string debug_name() const noexcept override {
        return "<opaque:" + std::string(_name) + ">";
    }
#endif  // !defined(NDEBUG)

    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Builtin;
    }

    [[nodiscard]] util::Result<absl::Nonnull<Type*>> resolve(Scope& scope) override;
};

}  // namespace rain::lang::ast
