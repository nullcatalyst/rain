#pragma once

#include "rain/lang/ast/type/type.hpp"
#include "rain/util/maybe_owned_ptr.hpp"

namespace rain::lang::ast {

class OptionalType : public Type {
    absl::Nonnull<Type*> _type;

  public:
    OptionalType(absl::Nonnull<Type*> type) : _type(type) {}
    ~OptionalType() override = default;

    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Optional;
    }
    [[nodiscard]] std::string display_name() const noexcept override {
        return absl::StrCat("?", _type->display_name());
    }

    [[nodiscard]] constexpr const Type& type() const noexcept { return *_type; }
    [[nodiscard]] constexpr Type&       type() noexcept { return *_type; }
};

}  // namespace rain::lang::ast
