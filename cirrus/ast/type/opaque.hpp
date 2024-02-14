#pragma once

#include "cirrus/ast/type/type.hpp"
#include "cirrus/util/string.hpp"

namespace cirrus::ast {

struct OpaqueType : public Type {
    util::String _name;

  public:
    OpaqueType(util::String name) : _name{std::move(name)} {}

    [[nodiscard]] static std::shared_ptr<OpaqueType> alloc(util::String name) {
        return std::make_shared<OpaqueType>(std::move(name));
    }

    [[nodiscard]] NodeKind kind() const noexcept override { return NodeKind::OpaqueType; }

    [[nodiscard]] util::String name() const noexcept { return _name; }
};

}  // namespace cirrus::ast
