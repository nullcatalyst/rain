#pragma once

#include "rain/lang/ast/type/type.hpp"

namespace rain::lang::ast {

class MetaType : public Type {
    absl::Nonnull<Type*> _type;

  public:
    MetaType(absl::Nonnull<Type*> type);
    ~MetaType() override = default;

    // Type
    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Meta;
    }
    [[nodiscard]] std::string display_name() const noexcept override {
        return absl::StrCat("<meta:", _type->display_name(), ">");
    }
    [[nodiscard]] constexpr lex::Location location() const noexcept override {
        // Meta types do not have a location. They are implicitly defined.
        return lex::Location();
    }

    // MetaType
    [[nodiscard]] constexpr const Type& type() const noexcept { return *_type; }
    [[nodiscard]] constexpr Type&       type() noexcept { return *_type; }

  protected:
    [[nodiscard]] util::Result<absl::Nonnull<Type*>> _resolve(Options& options,
                                                              Scope&   scope) override;
};

}  // namespace rain::lang::ast
