#pragma once

#include "cirrus/ast/type/type.hpp"
#include "cirrus/util/string.hpp"

namespace cirrus::ast {

struct UnresolvedType : public Type {
    util::String _name;

  public:
    UnresolvedType(util::String name) : _name{std::move(name)} {}

    [[nodiscard]] static std::shared_ptr<UnresolvedType> alloc(util::String name) {
        return std::make_shared<UnresolvedType>(std::move(name));
    }

    [[nodiscard]] constexpr TypeKind kind() const noexcept override {
        return TypeKind::UnresolvedType;
    }

    [[nodiscard]] util::String name() const noexcept { return _name; }
};

}  // namespace cirrus::ast
