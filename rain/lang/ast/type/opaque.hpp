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
    OpaqueType(std::string_view name);
    ~OpaqueType() override = default;

    // Type
    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Builtin;
    }
    [[nodiscard]] std::string display_name() const noexcept override { return std::string(_name); }
    [[nodiscard]] constexpr lex::Location location() const noexcept override {
        // Opaque types do not have a location. They are implicitly defined.
        return lex::Location();
    }

  protected:
    [[nodiscard]] util::Result<absl::Nonnull<Type*>> _resolve(Options& options,
                                                              Scope&   scope) override;
};

}  // namespace rain::lang::ast
