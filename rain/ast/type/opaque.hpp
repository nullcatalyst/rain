#pragma once

#include "rain/ast/type/type.hpp"
#include "rain/util/string.hpp"

namespace rain::ast {

struct OpaqueType : public Type {
    util::String _name;

  public:
    OpaqueType(util::String name) : _name{std::move(name)} {}

    [[nodiscard]] static std::shared_ptr<OpaqueType> alloc(util::String name) {
        return std::make_shared<OpaqueType>(std::move(name));
    }

    [[nodiscard]] constexpr TypeKind kind() const noexcept override { return TypeKind::OpaqueType; }

    [[nodiscard]] util::String name() const noexcept { return _name; }
};

}  // namespace rain::ast
