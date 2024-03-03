#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "rain/ast/type/type.hpp"

namespace rain::ast {

struct InterfaceTypeMethodData {
    util::String name;
    TypePtr      type;
};

struct InterfaceType : public Type {
    util::String                         _name;
    std::vector<InterfaceTypeMethodData> _methods;

  public:
    InterfaceType(util::String name, std::vector<InterfaceTypeMethodData> methods)
        : _name{std::move(name)}, _methods{std::move(methods)} {}

    [[nodiscard]] static std::shared_ptr<InterfaceType> alloc(
        util::String name, std::vector<InterfaceTypeMethodData> methods) {
        return std::make_shared<InterfaceType>(std::move(name), std::move(methods));
    }

    [[nodiscard]] constexpr TypeKind kind() const noexcept override {
        return TypeKind::InterfaceType;
    }

    [[nodiscard]] util::String name() const noexcept { return _name; }
    [[nodiscard]] const std::vector<InterfaceTypeMethodData>& methods() const& noexcept {
        return _methods;
    }
    [[nodiscard]] std::vector<InterfaceTypeMethodData>&& methods() && noexcept {
        return std::move(_methods);
    }
};

}  // namespace rain::ast
