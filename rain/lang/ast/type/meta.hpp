#pragma once

#include "rain/lang/ast/type/type.hpp"
#include "rain/util/maybe_owned_ptr.hpp"

namespace rain::lang::ast {

class MetaType : public Type {
    absl::Nonnull<Type*> _type;

  public:
    MetaType(absl::Nonnull<Type*> type) : _type(type) {}
    ~MetaType() override = default;

    // Type
    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Meta;
    }
    [[nodiscard]] std::string name() const noexcept override {
        return absl::StrCat("<meta:", _type->name(), ">");
    }
    [[nodiscard]] constexpr lex::Location location() const noexcept override {
        // Meta types do not have a location. They are implicitly defined.
        return lex::Location();
    }

    // MetaType
    [[nodiscard]] constexpr const Type& type() const noexcept { return *_type; }
    [[nodiscard]] constexpr Type&       type() noexcept { return *_type; }

    [[nodiscard]] util::Result<absl::Nonnull<Type*>> resolve(Options& options,
                                                             Scope&   scope) override;
};

}  // namespace rain::lang::ast
